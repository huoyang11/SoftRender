#pragma once

#include "image.h"
#include "glm/glm.hpp"

#include <functional>
#include <vector>
#include <string>
#include <unordered_map>

class vertex_buffer{
public:
    vertex_buffer(void* b,uint32_t size);
    ~vertex_buffer();

    void* get_buffer() const;
    void set_item_size(uint32_t item_size);
    uint32_t get_item_size() const;
    uint32_t get_size() const;
    void* get_index(uint32_t index) const;

private:
    void* buffer;
    uint32_t size;
    uint32_t item_size;
};

class vertex_index_buffer{
public:
    vertex_index_buffer(uint32_t* buffer, uint32_t size);
    ~vertex_index_buffer();
    uint32_t* get_buffer() const;
    uint32_t get_size() const;

private:
    uint32_t* buffer;
    uint32_t size;
};

struct VS_IN
{
    void* data;
};

struct VS_OUT
{
    glm::vec4 position;
    std::vector<float> out_to_fragment;
};

struct FG_OUT
{
    const glm::vec4 position;
    glm::vec4 color;
};

struct texture
{
    void* data;
    int w;
    int h;
};

struct triangle {
    VS_OUT vertex[3];
};

class render;
using vertex_shader_callback = std::function<void(render *r,const VS_IN &data, VS_OUT &out_data)>;
using fragment_shader_callback = std::function<void(render* r, const VS_OUT &data, FG_OUT &out_data)>;

class render
{
public:
    render();
    ~render();

    void set_vertex_shader_callback(vertex_shader_callback fn);

    void set_fragment_shader_callback(fragment_shader_callback fn);

    void set_unifrom(std::string name,void *data);

    void* get_unifrom(std::string name);

    void draw_line(const glm::vec2 &p1,const glm::vec2 &p2);

    void draw_triangle();

    void draw_triangle_ex();

    const image &get_image() const;

    void viewport(uint16_t w,uint16_t h);

    struct texture load_texture(const std::string &path);

    glm::vec4 texture2D(struct texture*t,const glm::vec2& uv);

    glm::vec4 viewport_vec(const glm::vec4& v);

    void set_vertex_buffer(vertex_buffer* vb);

    void set_vertex_index_buffer(vertex_index_buffer *vib);

    void clean();

private:
    //顶点着色器
    void vertex();
    //裁剪
    void cliping();
    //屏幕映射
    void screen_mapping();
    //三角形遍历
    void for_each_triangle();

    void interpolation(const VS_OUT& A, const VS_OUT& B, const VS_OUT& C, glm::vec3& point);

    void scanning_triangle(const VS_OUT& A, const VS_OUT& B, const VS_OUT& C);
    //片元着色器
    void fragment();
    //合并
    void merge();

    VS_OUT get_point_vs_out(float a,float b,float c, const triangle& t, const glm::vec3 &point);
private:
    static float* zBuffer1;
    static float* zBuffer2;
    //static std::vector<VS_OUT> vs_outs;
    static std::vector<VS_OUT> fragments;
    static std::vector<FG_OUT> fg_outs;
    static std::vector<triangle> triangles;
    static std::unordered_map<std::string, void*> unifroms;

private:
    uint16_t m_w;
    uint16_t m_h;

    image m_img;
    vertex_buffer *m_vb;
    vertex_index_buffer *m_vib;
    vertex_shader_callback m_vsfn;
    fragment_shader_callback m_fsfn;
};