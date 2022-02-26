#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include "includes.h"

#include "util.h"

Shader::Shader(const std::vector<std::string> &paths) :
	m_paths(paths),
	m_program(0),
	m_ok(false)
{
	load();
}

Shader::~Shader()
{
	glDeleteProgram(m_program);
}

void Shader::use()
{
	if(m_ok)
	{
		float time_value = glfwGetTime();
		int time_location = glGetUniformLocation(m_program, "time");
		glUseProgram(m_program);
		glUniform1f(time_location, time_value);
	}
}

void Shader::reload()
{
	load();
}

bool Shader::set_float(const std::string &name, float value)
{
	GLuint location;
	if(!get_location(name, location))
		return false;
	glUniform1f(location, value);
	return true;
}

bool Shader::set_uint(const std::string &name, GLuint value)
{
	GLuint location;
	if(!get_location(name, location))
		return false;
	glUniform1i(location, value);
	return true;
}

bool Shader::set_mat4(const std::string &name, const glm::mat4 &mat)
{
	GLuint location;
	if(!get_location(name, location))
		return false;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	return true;
}

bool Shader::set_vec3(const std::string &name, const glm::vec3 &vec)
{
	GLuint location;
	if(!get_location(name, location))
		return false;
	glUniform3fv(location, 1, glm::value_ptr(vec));
	return true;
}

void Shader::reset()
{
	if(!m_ok)
		return;
	glDeleteProgram(m_program);
	m_program = 0;
	m_ok = false;
}

void Shader::load()
{
	static std::map<const std::string, GLenum> EXT_TO_TYPE = {
		{".vert", GL_VERTEX_SHADER},
		{".frag", GL_FRAGMENT_SHADER},
	};
	GLuint program = glCreateProgram();
	std::vector<GLuint> shaders;
	for(const auto &path : m_paths)
	{
		GLenum shader_type = 0;
		for(auto iter = EXT_TO_TYPE.begin(); iter != EXT_TO_TYPE.end(); iter++)
		{
			if(path.find(iter->first) != -1)
			{
				shader_type = iter->second;	
				break;
			}
		}
		if(!shader_type)
		{
			error_msg("shader file has unknown extention", nullptr, path.c_str());
			break;
		}
		GLuint shader = glCreateShader(shader_type);
		std::string shader_src = read_file(path);
		if (shader_src.empty()) {
			error_msg("can't open shader file", nullptr, path.c_str());
			break;
		}

		char* raw_src = const_cast<char*>(shader_src.c_str());
		glShaderSource(shader, 1, &raw_src, NULL);
		glCompileShader(shader);
		if(!check_status(shader, GL_COMPILE_STATUS, "can't compile shader", path.c_str()))
			break;

		shaders.push_back(shader);
		glAttachShader(program, shader);
	}

	if(m_paths.size() != shaders.size())
	{
		delete_shaders(shaders);
		glDeleteProgram(program);
		return;
	}
	
	glLinkProgram(program);
	delete_shaders(shaders);
	if(!check_status(program, GL_LINK_STATUS, "can't link shader program"))
	{
		glDeleteProgram(program);
		return;
	}

	m_program = program;
	m_ok = true;
}

void Shader::delete_shaders(std::vector<GLuint> &shaders)
{
	for(const auto &shader : shaders)
		glDeleteShader(shader);
}

bool Shader::check_status(GLuint object, GLuint status, const char *msg, const char *path)
{
	int success;
	glGetShaderiv(object, status, &success);
	if(!success)
	{
		int len;
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &len);
		std::vector<char> info_log(len);
		switch(status)
		{
		case GL_COMPILE_STATUS:
			glGetShaderInfoLog(object, len, NULL, info_log.data());
			std::cout << info_log.data() << std::endl;
			break;
		case GL_LINK_STATUS:
			glGetProgramInfoLog(object, len, NULL, info_log.data());
			break;
		default:
			assert(false);
		}
		error_msg(msg, info_log.data(), path);
	}
	return success;
}

bool Shader::get_location(const std::string &name, GLuint &location)
{
	if(!m_ok)
		return false;
	location = glGetUniformLocation(m_program, name.c_str());
	if(location == GL_INVALID_VALUE || location == GL_INVALID_OPERATION)
		return false;
	return true;
}
