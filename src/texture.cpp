#include <iostream>
#include <cstdlib>

#include <vector>

#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <thread>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include "texture.h"

#include "util.h"


Texture::Texture(const std::string &path) :
	Component(Component::TEXTURE),
	m_path(path),
	m_ok(false)
{
	load();
}

Texture::~Texture()
{
	reset();
}

void Texture::reload()
{
	load();
}

void Texture::use(int num, std::string name, std::shared_ptr<Shader> shader)
{
    m_mutex.lock();
    if(m_tex_data.get())
        apply_data();
	if(m_ok)
	{
        glActiveTexture(GL_TEXTURE0 + num);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        shader->set_uint(name, num);
    }
    m_mutex.unlock();
}

void Texture::reset()
{
    m_mutex.lock();
	if(m_ok)
    {
        if(m_tex_data.get())
            stbi_image_free(m_tex_data->data);
        if(m_thread.get())
            m_thread->join();
        glDeleteTextures(1, &m_texture);
        m_ok = false;
    }
    m_mutex.unlock();
}

void Texture::load()
{
    m_thread.reset(new std::thread(Texture::async_load, this, m_path));
    m_thread->detach();
}

void Texture::async_load(Texture *texture, std::string path)
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if(data == nullptr)
	{
		error_msg("Can't open image", nullptr, path.c_str());
		return;
	}
    texture->on_loaded(width, height, nrChannels, data);
}

void Texture::on_loaded(int width, int height, int nrChannels, unsigned char *data)
{
    m_mutex.lock();
    m_thread.reset();
    m_tex_data.reset(new TextureData{width, height, nrChannels, data});
    m_mutex.unlock();
}

void Texture::apply_data()
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_tex_data->width, m_tex_data->height, 
           0, GL_RGB, GL_UNSIGNED_BYTE, m_tex_data->data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(m_tex_data->data);
    m_tex_data.reset();

	glBindTexture(GL_TEXTURE_2D, 0);
	m_texture = texture;
	m_ok = true;
}
