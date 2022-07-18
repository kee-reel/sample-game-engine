#ifndef H_SCRIPT_
#define H_SCRIPT_
#include "includes.h"

#include <sol/sol.hpp>

namespace sge
{

class Script
{
public:
    Script(std::string path) :
        m_path(path)
    {
        if(m_path.empty())
            throw std::runtime_error("empty filename");
    }

    std::unique_ptr<sol::environment> make_env(sol::state_view &lua)
    {
        if(!m_bytecode)
            load_script(lua);
        const std::string_view source = m_bytecode->as_string_view();
        auto&& env = std::make_unique<sol::environment>(lua, sol::create, lua.globals());
        const sol::protected_function_result load_res = lua.safe_script(source, *env);
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

};
#endif
