#ifndef SOGL_H_
#define SOGL_H_

#include <memory>
#include <vector>
#include <string>

namespace sge
{

class ITransform
{
public:
	virtual void set_pos(float x, float y, float z) = 0;
	virtual void set_rot(float x, float y, float z) = 0;
	virtual void set_scale(float x, float y, float z) = 0;
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
	virtual std::shared_ptr<IGameObject> add_game_object(const std::string &material) = 0;
};

std::shared_ptr<IApplication> instance();

};

#endif
