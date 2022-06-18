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
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	m_camera.reset(new Camera(m_width, m_height));

    auto objects = config["objects"];
    for(auto iter = objects.begin(); iter != objects.end(); iter++)
        add_game_object(iter.value());

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

	// Draw calls
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(auto material_to_mesh = m_game_objects.begin(); material_to_mesh != m_game_objects.end(); material_to_mesh++)
	{
        auto &&material = material_to_mesh->first;
		material->use(m_camera, m_lights);
		for(auto mesh_to_go = material_to_mesh->second.begin(); mesh_to_go != material_to_mesh->second.end(); mesh_to_go++)
		{
			for(auto &go : mesh_to_go->second)
			{
                go->update();
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
template<class T>
T get_value(nlohmann::basic_json<>& config, const char *name)
{
    auto iter = config.find(name);
    if(iter == config.end())
        return T{};
    return *iter;
}

std::shared_ptr<GameObject> Application::add_game_object(nlohmann::basic_json<>& config)
{
    std::cout << config << std::endl;
	std::shared_ptr<Material> material = ResourceLoader::instance().get_material(get_value<std::string>(config, "material"));
	std::shared_ptr<Mesh> mesh = ResourceLoader::instance().get_mesh();
	auto &mesh_to_go = m_game_objects[material];
	auto &&go = std::shared_ptr<GameObject>(new GameObject(get_value<std::string>(config, "script")));
    std::string light_path = get_value<std::string>(config, "light");
    if(!light_path.empty())
    {
        auto &&light = std::shared_ptr<Light>(new Light(light_path, go));
        m_lights.push_back(light);
    }
	mesh_to_go[mesh].push_back(go);
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
        for(auto material_to_mesh = m_game_objects.begin(); material_to_mesh != m_game_objects.end(); material_to_mesh++)
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
