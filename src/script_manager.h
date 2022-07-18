#ifndef SCRIPT_MANAGER_H_
#define SCRIPT_MANAGER_H_

#include <sol/sol.hpp>
#include <barrier>
#include <thread>

#include "iscript_manager.h"
#include "includes.h"
#include "script_state.h"
#include "game_object.h"
#include "transform.h"
#include "application.h"
#include "tick_limiter.h"

namespace sge
{

class ScriptManager : public IScriptManager
{
    using ScriptStates = std::vector<std::shared_ptr<ScriptState>>;
    using ScriptPool = std::pair<sol::state, std::shared_ptr<ScriptStates>>;
public:
    ScriptManager() = delete;
    ScriptManager(const ScriptManager &other) = delete;
    ScriptManager(ScriptManager &&other) = delete;
    ScriptManager& operator=(const ScriptManager &other) = delete;
    ScriptManager& operator=(ScriptManager &&other) = delete;

    ScriptManager(Application *app);
    void add_script(std::shared_ptr<GameObject> go, const std::string &script_path) override;
    void start() override;
    void stop() override;

private:
    Application *m_app;
    size_t m_threads_count;
    std::vector<std::shared_ptr<ScriptPool>>::iterator m_scripts_pool_selector;
    std::vector<std::shared_ptr<ScriptPool>> m_scripts;
    std::atomic<bool> m_stopping;
    std::vector<std::thread> m_script_threads;
    std::barrier<TickLimiter> m_sync_barrier;
};

};

#endif
