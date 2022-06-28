#include "application.h"

#include <functional>
#include <latch>
#include <barrier>
#include <thread>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "tick_limiter.h"
#include "includes.h"
#include "util.h"
#include "ECS/transform.h"
#include "ECS/render.h"
#include "ECS/script.h"
#include "ECS/render_system.h"
#include "ECS/script_system.h"

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
	m_is_quitting(false)
{
    m_ecs.add_system<ecs::Render, ecs::RenderSystem>();
    m_ecs.add_system<ecs::Script, ecs::ScriptSystem>();
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
    glEnable(GL_CULL_FACE);  
	glEnable(GL_DEPTH_TEST);
	m_camera.reset(new Camera(m_width, m_height));

    res::Loader::instance().set_base_path(config["assets_path"]);
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
    try
    {
        start();
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

	return true;
}

void Application::fini()
{
	glfwTerminate();
}

void Application::start()
{
    TickLimiter limiter(1000.f/60);
    int i = 0;
    while(!m_is_quitting.load())
    {
        handle_controls();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_ecs.update(m_camera, m_lights);
        for(auto material_to_mesh = m_material_to_mesh.begin(); material_to_mesh != m_material_to_mesh.end(); material_to_mesh++)
        {
            auto &&material = material_to_mesh->first;
            material->use(m_camera, m_lights);
            continue;
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
        limiter();
        if(i++ > 10)
        {
            i = 0;
            std::cout << (limiter.dt() ? static_cast<int>(1000.0f / limiter.dt()) : 0) << " FPS" << std::endl;
        }
    }
}

std::shared_ptr<GameObject> Application::add_game_object(
        std::string &&material_path, std::string &&script_path, std::string &&light_path)
{
    auto entity = m_ecs.add_entity();
    m_ecs.add_component<ecs::Transform>(entity);

	auto &&go = std::shared_ptr<GameObject>(new GameObject());
    m_game_objects[go->uid] = go;

    if(!material_path.empty())
    {
        std::shared_ptr<Material> material = res::Loader::instance().get_material(material_path);
        std::shared_ptr<Mesh> mesh = res::Loader::instance().get_mesh();
        auto &mesh_to_go = m_material_to_mesh[material];
        mesh_to_go[mesh].push_back(go);

        m_ecs.add_component<ecs::Render>(entity, material, mesh);
    }

    if(!script_path.empty())
    {
        //(*m_scripts_pool_selector)->second->push_back(std::make_shared<ScriptState>(
        //        go, 
        //        (*m_scripts_pool_selector)->first, 
        //        res::Loader::instance().get_script(script_path)
        //));
        //m_scripts_pool_selector++;
        //if(m_scripts_pool_selector == m_scripts.end())
        //    m_scripts_pool_selector = m_scripts.begin();

        m_ecs.add_component<ecs::Script>(entity, res::Loader::instance().get_script(script_path));
    }

    if(!light_path.empty())
    {
        m_lights.push_back(res::Loader::instance().get_light(go, light_path));
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
	m_is_quitting.store((!is_fullscreen && is_key_pressed(GLFW_KEY_ESCAPE)) || glfwWindowShouldClose(m_window));

	if(m_is_quitting.load())
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
        m_camera->move(glm::vec3(x, y, z));
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
