#ifndef H_SCRIPT_STATE_
#define H_SCRIPT_STATE_
#include "includes.h"

#include "component.h"
#include "script.h"

#include <sol/sol.hpp>


class ScriptState : public Component
{
public:
    ScriptState(std::shared_ptr<GameObject> game_object, sol::state_view lua, std::shared_ptr<res::Script> script) :
        Component(Component::SCRIPT),
        m_game_object(std::move(game_object)),
        m_script(std::move(script)),
        m_lua(std::move(lua)),
        m_env(m_script->make_env(m_lua))
    {
        (*m_env)["self"] = m_game_object;
        (*m_env)["init"]();
    }

    void update()
    {
        (*m_env)["update"]();
    }

private:
    std::shared_ptr<GameObject> m_game_object;
    sol::state_view m_lua;
    std::shared_ptr<res::Script> m_script;
    std::unique_ptr<sol::environment> m_env;
};
#endif
