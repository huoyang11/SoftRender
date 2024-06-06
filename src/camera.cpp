#include "camera.h"
#include <glm/gtc/type_ptr.hpp>

camera::camera(float fov,float aspect,float zNear,float zFar, project_mode m):
    m_fov(fov),
    m_aspect(aspect),
    m_zNear(zNear),
    m_zFar(zFar),
    mode(m),
    m_position(0.0, 0.0, 0.0),
    m_rotition(0.0, 0.0, 0.0)
{

}

camera::~camera()
{

}

void camera::set_fov(float fov)
{
    m_fov = fov;
}

void camera::set_aspect(float aspect)
{
    m_aspect = aspect;
}

void camera::set_zNear(float zNear)
{
    m_zNear = zNear;
}

void camera::set_zFar(float zFar)
{
    m_zFar = zFar;
}

void camera::set_mode(project_mode m)
{
    mode = m;
}

void camera::set_position(const glm::vec3 &pos)
{
    m_position = pos;
}

void camera::set_rotition(const glm::vec3 &rot)
{
    m_rotition = rot;
}

glm::mat4 camera::get_view_mat()
{
    glm::mat4 ret(1.0);
    ret = glm::translate(ret,m_position);
    ret = glm::rotate(ret,glm::radians(m_rotition.x),glm::vec3(1.0,0.0,0.0));
    ret = glm::rotate(ret,glm::radians(m_rotition.y),glm::vec3(0.0,1.0,0.0));
    ret = glm::rotate(ret,glm::radians(m_rotition.z),glm::vec3(0.0,0.0,1.0));

    return glm::inverse(ret);
}

glm::mat4 camera::get_project_mat()
{
    if (mode == ORTHOGRAPHIC) {
        //todo
        return glm::ortho(-m_aspect, m_aspect,-1.0f,1.0f, m_zNear,m_zFar);
    } else if (mode == PERSPECTIVE) {
        return glm::perspective(m_fov,m_aspect,m_zNear,m_zFar);
    }

    assert(false);
    return glm::mat4();
}