#include <csignal>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "application.h"
#include "tick_limiter.h"

#include "includes.h"
#include "util.h"
#include "timer.h"

#include <boost/asio.hpp>

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
        m_script_manager = IScriptManager::make(this);

        Loader::instance().set_base_path(config["assets_path"]);
        auto objects = config["objects"];
        for(auto iter = objects.begin(); iter != objects.end(); iter++)
        {
            auto &&object_config = iter.value();
            add_game_object(
                get_value<std::string>(object_config, "model"),
                get_value<std::string>(object_config, "material"),
                get_value<std::string>(object_config, "script"),
                get_value<std::string>(object_config, "light"),
                get_value<bool>(object_config, "flip_uv")
            );
        }

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
        m_script_manager->start();

        TickLimiter render_limit(1000/60.);
        while(!m_is_quitting)
        {
            //Timer t1("loop");
            handle_controls();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for(auto& [material, meshes] : m_material_to_mesh)
            {
                //Timer t1("material");
                material->use(m_camera, m_lights);
                for(auto& [mesh, game_objects] : meshes)
                {
                    //Timer t1("mesh");
                    for(auto &game_object : game_objects)
                    {
                        material->use_model(game_object->transform.get_model());
                        mesh->draw();
                    }
                }
            }
            glfwSwapBuffers(m_window);
            render_limit();
            std::cout << "FPS:" << (render_limit.dt() ? (1000 / render_limit.dt()) : 0) << std::endl;
        }

        m_script_manager->stop();
    }

    std::shared_ptr<GameObject> Application::add_game_object(std::string &&model_path, std::string &&material_path,
            std::string &&script_path, std::string &&light_path, bool flip_uv)
    {
        //auto entity = m_ecs.add_entity();
        //m_ecs.add_component<ecs::Transform>(entity);

        auto go = std::make_shared<GameObject>();
        m_game_objects.push_back(go);

        if(!model_path.empty())
        {
            std::shared_ptr<Model> model = Loader::instance().get_model(model_path, flip_uv);
            auto &material_to_meshes = model->get_material_to_mesh();
            if(material_to_meshes.size() == 0)
            {
                if(material_path.empty())
                    throw std::runtime_error("Model has no materials specified");
                std::shared_ptr<Material> material = Loader::instance().get_material(material_path);
                auto &meshes = m_material_to_mesh[material];
                auto &model_meshes = model->get_meshes();
                for(auto &mesh : model_meshes)
                    meshes[mesh].push_back(go);
            }
            else
            {
                for(auto& [material, model_meshes] : material_to_meshes)
                {
                    auto &meshes = m_material_to_mesh[material];
                    for(auto &mesh : model_meshes)
                        meshes[mesh].push_back(go);
                }
            }
            //m_ecs.add_component<ecs::Render>(entity, material, mesh);
        }

        if(!script_path.empty())
            m_script_manager->add_script(go, std::move(script_path));

        if(!light_path.empty())
            m_lights.push_back(Loader::instance().get_light(go, light_path));

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

        if(is_key_pressed(GLFW_KEY_F))
        {
            toggle_fullscreen(true);
        }
        else if(is_key_pressed(GLFW_KEY_ESCAPE))
        {
            toggle_fullscreen(false);
        }
        else if(is_key_pressed(GLFW_KEY_B))
            std::raise(SIGINT);


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
