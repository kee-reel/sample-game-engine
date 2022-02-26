#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "shader.h"
#include "component.h"

class Texture : public Component
{
    struct TextureData
    {
        int width;
        int height;
        int nrChannels;
        unsigned char *data;
    };
public:
	Texture(const std::string &path);
	~Texture();
	void reload();
	void use(int num, std::string name, std::shared_ptr<Shader> shader);

private:
	void reset();
	void load();
    static void async_load(Texture *texture, std::string path);
    void on_loaded(int width, int height, int nrChannels, unsigned char *data);
    void apply_data();

private:
    std::mutex m_mutex;
	bool m_ok;
    std::unique_ptr<TextureData> m_tex_data;
	std::string m_path;
	GLuint m_texture;
    std::unique_ptr<std::thread> m_thread;
};
#endif
