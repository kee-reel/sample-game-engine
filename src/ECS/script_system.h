#ifndef ECS_SCRIPT_SYSTEM_
#define ECS_SCRIPT_SYSTEM_

#include "system.h"
#include "script.h"

using ScriptPool = std::pair<sol::state, std::shared_ptr<std::set<ecs::Component*>>>;

namespace ecs
{

class ScriptSystem : public System
{
public:
    ScriptSystem(std::shared_ptr<std::set<Component*>> components) :
        System{components}
    {
        auto threads_count = std::thread::hardware_concurrency();
        std::cout << threads_count << " concurrent threads are supported.\n";
        if(threads_count > 1)
            threads_count--;

        m_scripts.reserve(threads_count);
        m_scripts_pool_selector = m_scripts.begin();
        try
        {
            for(int i = 0; i < threads_count; i++)
            {
                auto data = std::make_shared<ScriptPool>(
                        std::make_pair(sol::state(), std::make_shared<std::set<Component*>>()));
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
                lua.new_usertype<Entity>("gameobject",
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
        auto sync_tick = std::make_shared<std::barrier<std::function<void()>>>(
                threads_count, TickLimiter(1000.f/10)
        );

        m_script_threads.reserve(threads_count);
        for(auto iter = m_scripts.begin(); iter != m_scripts.end(); iter++)
        {
            std::cout << "Create scripts pool for " << (*iter)->second->size() << std::endl;
            m_script_threads.emplace_back([&](
                        std::shared_ptr<ScriptPool> pool,
                        std::shared_ptr<std::barrier<std::function<void()>>> sync_tick) mutable
            {
                auto scripts = pool->second;
                while(true)
                {
                    for(auto iter = scripts->begin(); iter != scripts->end(); iter++)
                        static_cast<Script*>(*iter)->update();
                    sync_tick->arrive_and_wait();
                }
                sync_tick->arrive_and_wait();
            }, *iter, sync_tick);
        }
    }
    virtual ~ScriptSystem() {}
    void on_added(Component* component) override
    {
        auto script = static_cast<Script*>(component);
        script->init((*m_scripts_pool_selector)->first);
        (*m_scripts_pool_selector)->second->insert(component);
        m_scripts_pool_selector++;
        if(m_scripts_pool_selector == m_scripts.end())
            m_scripts_pool_selector = m_scripts.begin();
    }
    void on_removed(Component* component) override {}
    void update() override
    {
    }
private:
    std::vector<std::thread> m_script_threads;
    std::vector<std::shared_ptr<ScriptPool>> m_scripts;
    std::vector<std::shared_ptr<ScriptPool>>::iterator m_scripts_pool_selector;
};

};
#endif
