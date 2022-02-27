#ifndef COMPONENT_H_
#define COMPONENT_H_

static uint64_t id_gen = 0;

class Component
{
public:
    enum Type
    {
        GAME_OBJECT,
        MATERIAL,
        LIGHT,
        MESH,
        TEXTURE,
        CAMERA,
    };

	Component(Type type) :
        m_comp_type(type),
		uid(id_gen++)
	{}

public:
    const Type m_comp_type;
	const uint64_t uid;
};
#endif
