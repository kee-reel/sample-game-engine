#ifndef H_SCRIPT_
#define H_SCRIPT_
#include "includes.h"

#include "component.h"

#include <sol/sol.hpp>

class Script : public Component
{
public:
    Script(std::string path) :
        Component(Component::SCRIPT),
        m_path(path)
    {
        if(m_path.empty())
            throw std::runtime_error("empty filename");
    }

    sol::environment make_env(sol::state_view &lua)
    {
        if(!m_bytecode)
            load_script(lua);
        const std::string_view source = m_bytecode->as_string_view();
        sol::environment env(lua, sol::create, lua.globals());
        const sol::protected_function_result load_res = lua.safe_script(source, env);
        return env;
    }

private:
    void load_script(sol::state_view &lua)
    {
        const sol::load_result target = lua.load_file(m_path);
        const sol::protected_function bytecode = target.get<sol::protected_function>();
        m_bytecode = std::make_unique<sol::bytecode>(bytecode.dump());
    }

private:
    std::string m_path;
    std::unique_ptr<sol::bytecode> m_bytecode;
};
#endif
