#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "application.h"

#include "includes.h"
#include "util.h"

namespace sge
{

Application Application::s_instance;

bool init(std::string config_path)
{
    return Application::instance().init(config_path);
}

Application &Application::instance()
{
	return s_instance;
}

Application::Application() :
	m_frame_duration(1000./ 60),
	m_is_quitting(false)
{
}

Application::~Application()
{

}

bool Application::init(std::string config_path)
{
	if( !glfwInit() )
	{
		error_msg("Failed to initialize GLFW.");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	glfwSetErrorCallback(error_callback);

    std::ifstream f(config_path);
    json config;
    f >> config;
    f.close();

    auto window_config = config["window"];
	m_width = window_config["width"];
	m_height = window_config["height"];
	m_window_name = window_config["title"];

	m_window = glfwCreateWindow(m_width, m_height, m_window_name.c_str(), NULL, NULL);
	if( m_window == NULL ){
		error_msg("Failed to init GLFW window.");
		return false;
	}
	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(m_window, mouse_callback);

	glewExperimental = true;
    GLenum err;
	if ((err = glewInit()) != GLEW_OK) {
        std::cerr << glewGetErrorString(err) << std::endl;
		glfwTerminate();
		return false;
	}

	toggle_fullscreen(false);
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
    auto clear_color = window_config["clear_color"];
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
	glEnable(GL_DEPTH_TEST);
	m_camera.reset(new Camera(m_width, m_height));

    try
    {
        m_lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math);
        m_lua.new_usertype<Application>("app",
            sol::constructors<>(),
            "add_game_object", &Application::add_game_object
        );
        m_lua.new_usertype<glm::vec3>("vec3",
            sol::constructors<glm::vec3(float,float,float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z
        );
        m_lua.new_usertype<GameObject>("gameobject",
            sol::constructors<>(),
            "transform", sol::readonly(&GameObject::transform)
        );
        m_lua.new_usertype<Transform>("transform",
            sol::constructors<>(),
            "set_pos", &Transform::set_pos,
            "set_rot", &Transform::set_rot,
            "set_scale", &Transform::set_scale,

            "get_pos", &Transform::get_pos,
            "get_rot", &Transform::get_rot,
            "get_scale", &Transform::get_scale,

            "rotate", &Transform::rotate,
            "move", &Transform::move,

            "front", &Transform::front,
            "up", &Transform::up
        );
        m_lua["app"] = this;

    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    std::cout << "[OBJECTS]" << std::endl;
    auto objects = config["objects"];
    for(auto iter = objects.begin(); iter != objects.end(); iter++)
    {
        auto &&object_config = iter.value();
        add_game_object(
            get_value<std::string>(object_config, "material"),
            get_value<std::string>(object_config, "script"),
            get_value<std::string>(object_config, "light")
        );
    }

    std::cout << "[UPDATE]" << std::endl;
    while(update());

	return true;
}

void Application::fini()
{
	glfwTerminate();
}

bool Application::update()
{
	auto start = std::chrono::high_resolution_clock::now();

	handle_controls();

    for(auto iter = m_scripts.begin(); iter != m_scripts.end(); iter++)
        (*iter)->update();

	// Draw calls
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(auto material_to_mesh = m_material_to_mesh.begin(); material_to_mesh != m_material_to_mesh.end(); material_to_mesh++)
	{
        auto &&material = material_to_mesh->first;
		material->use(m_camera, m_lights);
		for(auto mesh_to_go = material_to_mesh->second.begin(); mesh_to_go != material_to_mesh->second.end(); mesh_to_go++)
		{
			for(auto &go : mesh_to_go->second)
			{
				material->use_model(go->transform.get_model());
				mesh_to_go->first->draw();
			}
		}
	}
	glfwSwapBuffers(m_window);

	// Sleep to sustain FPS
	auto time_passed = std::chrono::high_resolution_clock::now() - start;
	if(m_frame_duration > time_passed)
		std::this_thread::sleep_for(m_frame_duration - time_passed);
	m_dt = (start - m_prev_time) / 1000.f;
	m_prev_time = start;
	
	return !m_is_quitting;
}

std::shared_ptr<GameObject> Application::add_game_object(
        std::string &&material_path, std::string &&script_path, std::string &&light_path)
{
	auto &&go = std::shared_ptr<GameObject>(new GameObject());
    m_game_objects[go->uid] = go;

    if(!material_path.empty())
    {
        std::shared_ptr<Material> material = ResourceLoader::instance().get_material(material_path);
        std::shared_ptr<Mesh> mesh = ResourceLoader::instance().get_mesh();
        auto &mesh_to_go = m_material_to_mesh[material];
        mesh_to_go[mesh].push_back(go);
    }

    if(!script_path.empty())
    {
        m_scripts.push_back(std::make_shared<Script>(go, m_lua, script_path));
    }

    if(!light_path.empty())
    {
        auto &&light = std::shared_ptr<Light>(new Light(light_path, go));
        m_lights.push_back(light);
    }
	return go;
}

Transform &Application::get_camera_transform()
{
    return m_camera->transform();
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	if(Application::instance().m_camera)
	{
		Application::instance().m_camera->update_aspect(width, height);
	}
	auto monitor = glfwGetPrimaryMonitor();
	if(!monitor)
	{
		Application::instance().m_width = width;
		Application::instance().m_height = height;
	}
}

void Application::error_callback(int error, const char* description)
{
	error_msg(description);
}

void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(Application::instance().m_camera)
	{
		Application::instance().m_camera->mouse(xpos, ypos);
	}
}

int Application::is_key_pressed(int key)
{
	return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void Application::handle_controls()
{
	glfwPollEvents();

	bool is_fullscreen = glfwGetWindowMonitor(m_window) != nullptr;
	m_is_quitting = (!is_fullscreen && is_key_pressed(GLFW_KEY_ESCAPE)) || glfwWindowShouldClose(m_window); 

	if(m_is_quitting)
		return;

	if(is_key_pressed(GLFW_KEY_R))
	{
        for(auto material_to_mesh = m_material_to_mesh.begin(); material_to_mesh != m_material_to_mesh.end(); material_to_mesh++)
        {
            material_to_mesh->first->reload();
            for(auto mesh_to_go = material_to_mesh->second.begin(); mesh_to_go != material_to_mesh->second.end(); mesh_to_go++)
            {
                mesh_to_go->first->reload();
            }
        }
	}
	else if(is_key_pressed(GLFW_KEY_F))
	{
		toggle_fullscreen(true);
	}
	else if(is_key_pressed(GLFW_KEY_ESCAPE))
	{
		toggle_fullscreen(false);
	}

	int x = is_key_pressed(GLFW_KEY_D) - is_key_pressed(GLFW_KEY_A);
	int y = is_key_pressed(GLFW_KEY_LEFT_CONTROL) - is_key_pressed(GLFW_KEY_SPACE);
	int z = is_key_pressed(GLFW_KEY_W) - is_key_pressed(GLFW_KEY_S);
    if(x || y || z)
        m_camera->move(glm::vec3(x, y, z) * m_dt.count());
}

void Application::toggle_fullscreen(bool enabled)
{
	toggle_cursor(!enabled);
	auto monitor = glfwGetPrimaryMonitor();
	auto mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(m_window, enabled ? monitor : nullptr, 0, 0, 
			enabled ? mode->width : m_width,
			enabled ? mode->height : m_height, GLFW_DONT_CARE);
}

void Application::toggle_cursor(bool enabled)
{
	m_cursor_shown = enabled;
	glfwSetInputMode(m_window, GLFW_CURSOR, 
			m_cursor_shown ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED );
}

};
