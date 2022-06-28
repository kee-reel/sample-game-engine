#ifndef ECS_SCRIPT_H_
#define ECS_SCRIPT_H_

#include <sol/sol.hpp>

#include "component.h"
#include "../script.h"

namespace ecs
{

class Script : public Component
{
public:
	Script(std::weak_ptr<Entity> &&entity, std::shared_ptr<res::Script> script);
	virtual ~Script();
    void init(sol::state_view lua);
    void update();

private:
    std::unique_ptr<sol::state_view> m_lua;
    std::shared_ptr<res::Script> m_script;
    std::unique_ptr<sol::environment> m_env;
};

};
#endif
