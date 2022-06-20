#ifndef H_SCRIPT_
#define H_SCRIPT_
#include "includes.h"

#include "component.h"

#include <sol/sol.hpp>

class Script : public Component
{
public:
    Script(sol::state_view lua, std::string path) :
        Component(Component::SCRIPT),
        m_path(path),
        m_lua(std::move(lua))
    {
        if(m_path.empty())
            throw std::runtime_error("empty filename");
        load_script();
    }

    sol::environment make_env()
    {
        const std::string_view source = m_bytecode->as_string_view();
        sol::environment env(m_lua, sol::create, m_lua.globals());
        const sol::protected_function_result load_res = m_lua.safe_script(source, env);
        return env;
    }

private:
    void load_script()
    {
        try
        {
            const sol::load_result target = m_lua.load_file(m_path);
            const sol::protected_function bytecode = target.get<sol::protected_function>();
            m_bytecode = std::make_unique<sol::bytecode>(bytecode.dump());
        }
        catch(std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

private:
    std::string m_path;
    sol::state_view m_lua;
    std::unique_ptr<sol::bytecode> m_bytecode;
};
#endif
