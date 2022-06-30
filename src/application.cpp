#include <latch>
#include <barrier>
#include <thread>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "application.h"
#include "tick_limiter.h"

#include "includes.h"
#include "util.h"
//#include "ECS/transform.h"
//#include "ECS/render.h"

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

    auto threads_count = std::thread::hardware_concurrency();
    std::cout << threads_count << " concurrent threads are supported.\n";
    if(threads_count >= 2)
        threads_count--;
    m_scripts.reserve(threads_count);
    m_scripts_pool_selector = m_scripts.begin();
    try
    {
        for(int i = 0; i < threads_count; i++)
        {
            auto data = std::make_shared<ScriptPool>(std::make_pair(sol::state(), std::make_shared<ScriptStates>()));
            auto &lua = data->first;
            lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math);
            lua.new_usertype<Application>("app",
                sol::constructors<>(),
                "add_game_object", &Application::add_game_object
            );
            lua.new_usertype<glm::vec3>("vec3",
                sol::constructors<glm::vec3(float,float,float)>(),
                "x", &glm::vec3::x,
                "y", &glm::vec3::y,
                "z", &glm::vec3::z
            );
            lua.new_usertype<GameObject>("gameobject",
                sol::constructors<>(),
                "transform", sol::readonly(&GameObject::transform)
            );
            lua.new_usertype<Transform>("transform",
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
            lua["app"] = this;
            m_scripts.push_back(std::move(data));
        }
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

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
    int threads_count = m_scripts.size();
    auto start = std::chrono::high_resolution_clock::now();
    std::barrier sync_barrier(threads_count, TickLimiter(1000/10.));

    std::vector<std::thread> script_threads;
    script_threads.reserve(threads_count);
    for(auto iter = m_scripts.begin(); iter != m_scripts.end(); iter++)
    {
        std::cout << "Create scripts pool for " << (*iter)->second->size() << std::endl;
        script_threads.emplace_back([&](std::shared_ptr<ScriptPool> pool){
            auto scripts = pool->second;
            while(!m_is_quitting.load())
            {
                for(auto iter = scripts->begin(); iter != scripts->end(); iter++)
                    (*iter)->update();
                sync_barrier.arrive_and_wait();
            }
            auto _ = sync_barrier.arrive();
        }, *iter);
    }

    TickLimiter render_limit(1000/60.);
    int i = 0;
    while(!m_is_quitting.load())
    {
        handle_controls();
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
        i++;
        glfwSwapBuffers(m_window);
        render_limit();
        std::cout << "FPS:" << (render_limit.dt() ? (1000 / render_limit.dt()) : 0) << std::endl;
    }
    for (auto& thread : script_threads) {
        thread.join();
    }
}

std::shared_ptr<GameObject> Application::add_game_object(
        std::string &&material_path, std::string &&script_path, std::string &&light_path)
{
    //auto entity = m_ecs.add_entity();
    //m_ecs.add_component<ecs::Transform>(entity);

	auto &&go = std::shared_ptr<GameObject>(new GameObject());
    m_game_objects[go->uid] = go;

    if(!material_path.empty())
    {
        std::shared_ptr<Material> material = res::Loader::instance().get_material(material_path);
        std::shared_ptr<Mesh> mesh = res::Loader::instance().get_mesh();
        auto &mesh_to_go = m_material_to_mesh[material];
        mesh_to_go[mesh].push_back(go);

        //m_ecs.add_component<ecs::Render>(entity, material, mesh);
    }

    if(!script_path.empty())
    {
        (*m_scripts_pool_selector)->second->push_back(
                std::make_shared<ScriptState>(
                    go, (*m_scripts_pool_selector)->first, res::Loader::instance().get_script(script_path)));
        m_scripts_pool_selector++;
        if(m_scripts_pool_selector == m_scripts.end())
            m_scripts_pool_selector = m_scripts.begin();
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
