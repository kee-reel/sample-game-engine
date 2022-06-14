#ifndef SOGL_H_
#define SOGL_H_

#include <memory>
#include <vector>
#include <string>

#include <glm/glm.hpp>

namespace sge
{

class ITransform
{
public:
	virtual void set_pos(glm::vec3 pos) = 0;
	virtual void set_rot(glm::vec3 rot) = 0;
	virtual void set_scale(glm::vec3 scale) = 0;

	virtual const glm::vec3& get_pos() = 0;
	virtual const glm::vec3& get_rot() = 0;
	virtual const glm::vec3& get_scale() = 0;

	virtual const glm::vec3& front() = 0;
	virtual const glm::vec3& up() = 0;

    virtual void move(glm::vec3 diff) = 0;
    virtual void rotate(glm::vec3 diff) = 0;
};

class IGameObject
{
public:
	virtual ~IGameObject() {}
	virtual ITransform &get_transform() = 0;
};

class IApplication
{
public:
	virtual ~IApplication() {};
	virtual bool init(int width, int height, std::string &&window_name) = 0;
	virtual void fini() = 0;
	virtual bool draw() = 0;
	virtual std::shared_ptr<IGameObject> add_game_object(
            const std::string &material, const std::string &light) = 0;
    virtual ITransform &get_camera_transform() = 0;
};

std::shared_ptr<IApplication> instance();

};

#endif
