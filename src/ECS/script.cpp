#include "script.h"

#include "../includes.h"

namespace ecs
{
Script::Script(std::weak_ptr<Entity> &&entity, std::shared_ptr<res::Script> script) :
    Component(std::move(entity)),
    m_script(std::move(script))
{
}

Script::~Script()
{

}

void Script::init(sol::state_view lua)
{
    m_lua = std::make_unique<sol::state_view>(std::move(lua));
    m_env = m_script->make_env(*m_lua);
    //m_env->set("self") = m_entity;
    //m_env->get("init")();
}

void Script::update()
{
    //m_env->get("update")();
}
};
