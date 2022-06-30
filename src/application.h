#include <atomic>
#include <sol/sol.hpp>

#include "includes.h"
#include "resource_loader.h"
#include "camera.h"
#include "material.h"
#include "mesh.h"
#include "light.h"
#include "game_object.h"
#include "script_state.h"

//#include "ECS/ecs.h"

#include "sge.h"


namespace sge
{

using mesh_to_go_t = std::map<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<GameObject>>>;
using material_to_mesh_t = std::map<std::shared_ptr<Material>, mesh_to_go_t>;
using ScriptStates = std::list<std::shared_ptr<ScriptState>>;
using ScriptPool = std::pair<sol::state, std::shared_ptr<ScriptStates>>;
class Application
{
public:
	Application();
	virtual ~Application();
	bool init(std::string config_path);
	void fini();
	void start();
	std::shared_ptr<GameObject> add_game_object(
            std::string &&material_path, std::string &&script_path, std::string &&light_path);
    Transform &get_camera_transform();
	static Application &instance();

private:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void error_callback(int error, const char* description);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	int is_key_pressed(int key);
	void handle_controls();
	void toggle_cursor(bool enabled);
	void toggle_fullscreen(bool enabled);

private:
    static Application s_instance;
	GLFWwindow* m_window;
	int m_width;
	int m_height;
    std::string m_window_name;
	bool m_cursor_shown;
    std::atomic<bool> m_is_quitting;

	std::shared_ptr<Camera> m_camera;
	material_to_mesh_t m_material_to_mesh;
    std::map<uint64_t, std::shared_ptr<GameObject>> m_game_objects;
    std::list<std::shared_ptr<Light>> m_lights;
    std::vector<std::shared_ptr<ScriptPool>>::iterator m_scripts_pool_selector;
    std::vector<std::shared_ptr<ScriptPool>> m_scripts;
    //ecs::ECS m_ecs;
};

};
