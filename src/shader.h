#ifndef SHADER_H_
#define SHADER_H_

#include "includes.h"

class Shader
{
public:
	Shader(const std::vector<std::string> &paths);
	virtual ~Shader();
	void reload();
	void use();
	bool set_float(const std::string &name, float value);
	bool set_uint(const std::string &name, GLuint value);
	bool set_mat4(const std::string &name, const glm::mat4 &mat);
	bool set_vec3(const std::string &name, const glm::vec3 &vec);
	bool set_vec4(const std::string &name, const glm::vec4 &vec);

private:
	void reset();
	void load();
	static void delete_shaders(std::vector<GLuint> &shaders);
	static bool check_status(GLuint object, GLuint status, const char *msg, const char *path=nullptr);
	bool get_location(const std::string &name, GLuint &location);

private:
	std::vector<std::string> m_paths;
	GLuint m_program;
	bool m_ok;
};

#endif
