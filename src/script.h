#ifndef H_SCRIPT_
#define H_SCRIPT_
#include "includes.h"

#include "component.h"

#include <sol/sol.hpp>


class Script : public Component
{
public:
    Script(std::shared_ptr<GameObject> game_object, sol::state_view lua, std::string script_path) :
        Component(Component::SCRIPT),
        m_game_object(std::move(game_object)),
        m_script_path(script_path),
        m_lua(std::move(lua)),
        m_env(m_lua, sol::create, m_lua.globals())
    {
        if(m_script_path.empty())
            throw std::runtime_error("empty filename");
        load_script();
    }

    void update()
    {
        m_env["update"]();
    }

private:
    void load_script()
    {
        try
        {
            m_env["self"] = m_game_object;
            m_lua.script_file(m_script_path, m_env);
        }
        catch(std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

private:
    std::shared_ptr<GameObject> m_game_object;
    std::string m_script_path;
    sol::state_view m_lua;
    sol::environment m_env;
};
#endif
