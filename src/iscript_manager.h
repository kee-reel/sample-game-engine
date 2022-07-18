#ifndef ISCRIPT_MANAGER_H_
#define ISCRIPT_MANAGER_H_

#include <sol/sol.hpp>

#include "includes.h"
#include "script_state.h"

namespace sge
{
class Application;

class IScriptManager
{
public:
    static std::unique_ptr<IScriptManager> make(Application *app);

    virtual void add_script(std::shared_ptr<GameObject> go, const std::string &script_path) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

};

#endif
