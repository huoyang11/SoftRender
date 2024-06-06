#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "render.h"
#include "camera.h"
#include "model.h"

#define WIDTH 1280
#define HEIGHT 720

static void vertex_shader(render* r, const VS_IN& data, VS_OUT& out_data)
{
    Vertex* vertex = (Vertex*)data.data;

    glm::mat4* project_mat = (glm::mat4*)r->get_unifrom("project_mat");
    glm::mat4* view_mat = (glm::mat4*)r->get_unifrom("view_mat");
    glm::mat4* model_mat = (glm::mat4*)r->get_unifrom("model_mat");

    out_data.position = (*project_mat) * (*view_mat) * (*model_mat) * glm::vec4(
        vertex->position[0], 
        vertex->position[1], 
        vertex->position[2],
        1);
    //uv
    out_data.out_to_fragment.push_back(vertex->texCoords[0]);
    out_data.out_to_fragment.push_back(vertex->texCoords[1]);
}

static void fragment_shader(render* r, const VS_OUT& data, FG_OUT& out_data)
{
    struct texture *container = (struct texture*)r->get_unifrom("container");

    float u = data.out_to_fragment.at(0);
    float v = data.out_to_fragment.at(1);

    out_data.color = r->texture2D(container,glm::vec2(u,v));
}

int main(int argc,char *argv[])
{
    window w;
    render r;
    r.viewport(WIDTH,HEIGHT);
    camera c((float)glm::radians(45.0),(float)1280/(float)720,0.1f,100.0f,PERSPECTIVE);
    c.set_position(glm::vec3(0.0,0.0,3.0));

    model m = model("../res/Cube.fbx");
    mesh *me = m.get_meshes().front();

    //mesh* me = new mesh(
    //    {
    //        {-1.0f, 1.0f,0.f, 0.f,1.0f},
    //        { 1.0f, 1.0f,0.f, 1.f,1.0f},
    //        {-1.0f,-1.0f,0.f, 0.f,0.0f},
    //        { 1.0f,-1.0f,0.f, 1.f,0.0f},
    //    },
    //    
    //    {0,2,1,2,1,3}
    //    );

    struct texture container = r.load_texture("../res/container2.png");

    float rot = 40.f;
    while (1)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        w.run();
        r.clean();

        r.set_vertex_buffer(me->get_vertex_buffer());
        r.set_vertex_index_buffer(me->get_vertex_index_buffer());
        r.set_vertex_shader_callback(vertex_shader);
        r.set_fragment_shader_callback(fragment_shader);

        glm::mat4 model_mat(1.0);
        model_mat = glm::rotate(model_mat,glm::radians(rot),glm::vec3(0.f,1.f,0.f));
        glm::mat4 view_mat = c.get_view_mat();
        glm::mat4 project_mat = c.get_project_mat();

        r.set_unifrom("model_mat",&model_mat);
        r.set_unifrom("view_mat", &view_mat);
        r.set_unifrom("project_mat", &project_mat);
        r.set_unifrom("container", &container);

        r.draw_triangle_ex();

        w.set_image(r.get_image());

        rot += 1;

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        float time_gap = std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()).count() - 
            std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
        std::cout << 1000.f / time_gap << std::endl;
    }
    

    return 0;
}