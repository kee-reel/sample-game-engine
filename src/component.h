#ifndef COMPONENT_H_
#define COMPONENT_H_
static unsigned long int id_gen = 0;

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
        m_type(type),
		uid(id_gen++)
	{}

public:
    const Type m_type;
	const unsigned long int uid;
};
#endif
