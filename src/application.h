#include "includes.h"
#include "resource_loader.h"
#include "camera.h"
#include "material.h"
#include "mesh.h"
#include "light.h"
#include "game_object.h"

#include "sge.h"

namespace sge
{

typedef std::map<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<GameObject>>> mesh_to_go_t;
typedef std::map<std::shared_ptr<Material>, mesh_to_go_t> material_to_mesh_t;

class Application
{
public:
	Application();
	virtual ~Application();
	bool init(std::string config_path);
	void fini();
	bool update();
	std::shared_ptr<GameObject> add_game_object(nlohmann::basic_json<>& config);
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
	bool m_is_quitting;

	std::chrono::high_resolution_clock::time_point m_prev_time;
	std::chrono::duration<float, std::milli> m_frame_duration, m_dt;

	std::shared_ptr<Camera> m_camera;
	material_to_mesh_t m_game_objects;
    std::list<std::shared_ptr<Light>> m_lights;
};

};
