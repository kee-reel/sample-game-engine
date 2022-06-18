#ifndef H_GAME_OBJECT_
#define H_GAME_OBJECT_
#include "includes.h"

#include "transform.h"
#include "component.h"

#include <sol/sol.hpp>


class GameObject : public Component
{
public:
    GameObject(std::string script_path) :
        Component(Component::GAME_OBJECT),
        m_script_path(script_path)
    {
        if(!m_script_path.empty())
            load_script();
    }

	virtual ~GameObject() {}

    void update()
    {
        if(lua)
            lua->operator[]("update")();
    }

private:
    void load_script()
    {
        try
        {
            lua.reset(new sol::state{});
            lua->open_libraries(sol::lib::base, sol::lib::io, sol::lib::math);
            lua->new_usertype<glm::vec3>("vec3",
                sol::constructors<glm::vec3(float,float,float)>()
            );
            auto gameobject_type = lua->new_usertype<GameObject>("gameobject",
                sol::constructors<>()
            );
            gameobject_type.set("transform", sol::readonly(&GameObject::transform));

            lua->new_usertype<Transform>("transform",
                sol::constructors<>(),
                "set_pos", &Transform::set_pos,
                "set_rot", &Transform::set_rot,
                "set_scale", &Transform::set_scale

                /*
                virtual const glm::vec3& get_pos() = 0;
                virtual const glm::vec3& get_rot() = 0;
                virtual const glm::vec3& get_scale() = 0;

                virtual const glm::vec3& front() = 0;
                virtual const glm::vec3& up() = 0;

                virtual void move(glm::vec3 diff) = 0;
                virtual void rotate(glm::vec3 diff) = 0;
                */
            );
            lua->operator[]("self") = this;
            lua->script_file(m_script_path);
        }
        catch(std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

public:
	Transform transform;
private:
    std::unique_ptr<sol::state> lua;
    std::string m_script_path;
};
#endif
