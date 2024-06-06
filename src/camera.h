#pragma once

#include "glm/glm.hpp"

enum project_mode{
    ORTHOGRAPHIC, //正交
    PERSPECTIVE   //透视
};

class camera {
public:
    camera(float fov,float aspect,float zNear,float zFar, project_mode m);
    ~camera();

    void set_fov(float fov);
    void set_aspect(float aspect);
    void set_zNear(float zNear);
    void set_zFar(float zFar);
    void set_mode(project_mode m);

    void set_position(const glm::vec3 &pos);
    void set_rotition(const glm::vec3 &rot);

    glm::mat4 get_view_mat();
    glm::mat4 get_project_mat();

private:
    float m_fov;                //视场角
    float m_aspect;             //屏幕宽高比
    float m_zNear;              //近平面
    float m_zFar;               //远平面

    project_mode mode;          //投影模式

    glm::vec3 m_position;        //位置
    glm::vec3 m_rotition;        //旋转(使用欧拉角)
};