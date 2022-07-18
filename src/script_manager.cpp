#include "script_manager.h"

namespace sge
{

std::unique_ptr<IScriptManager> IScriptManager::make(Application *app)
{
    return std::make_unique<ScriptManager>(app);
}

ScriptManager::ScriptManager(Application *app) :
    m_app{app},
    m_threads_count{std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() : 1},
    m_stopping{false},
    m_scripts{m_threads_count},
    m_script_threads{m_threads_count},
    m_sync_barrier{static_cast<ptrdiff_t>(m_threads_count), TickLimiter(1000/60.)}
{
    std::cout << m_threads_count << " concurrent threads are supported" << std::endl;

    for(int i = 0; i < m_scripts.size(); i++)
    {
        auto data = std::make_shared<ScriptPool>(std::make_pair(sol::state(), new ScriptStates{}));
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
        lua["app"] = m_app;
        m_scripts[i] = std::move(data);
    }
    m_scripts_pool_selector = m_scripts.begin();
}

void ScriptManager::add_script(std::shared_ptr<GameObject> go, const std::string& script_path)
{
    (*m_scripts_pool_selector)->second->push_back(std::make_shared<ScriptState>(
        go, (*m_scripts_pool_selector)->first, Loader::instance().get_script(script_path)));
    m_scripts_pool_selector++;
    if(m_scripts_pool_selector == m_scripts.end())
        m_scripts_pool_selector = m_scripts.begin();
}

void ScriptManager::start()
{
    int threads_count = m_scripts.size();
    auto start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < m_scripts.size(); i++)
    {
        std::shared_ptr<ScriptPool> &pool = m_scripts[i];
        std::cout << "Create scripts pool for " << pool->second->size() << std::endl;
        m_script_threads[i] = std::thread([&](std::shared_ptr<ScriptPool> pool){
            auto scripts = pool->second;
            while(!m_stopping.load())
            {
                for(auto iter = scripts->begin(); iter != scripts->end(); iter++)
                    (*iter)->update();
                m_sync_barrier.arrive_and_wait();
            }
            auto _ = m_sync_barrier.arrive();
        }, pool);
    }
}

void ScriptManager::stop()
{
    m_stopping = true;
    for(auto &t : m_script_threads)
        t.join();
}

};
