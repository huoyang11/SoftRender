#pragma once

#include <vector>

#include "render.h"

struct Vertex {
	float position[3];		//位置
	float texCoords[2];		//纹理坐标
	float normals[3];		//法线
	float tangent[3];		//切线
	float bitangent[3];     //副切线
};

class mesh {
public:
	mesh(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices);
	~mesh();

	inline uint32_t get_indices_count() const { return m_index_buffer->get_size(); };

	inline vertex_buffer* get_vertex_buffer() const { return m_vertex_buffer; };

	inline vertex_index_buffer* get_vertex_index_buffer() const { return m_index_buffer; };

private:
	vertex_buffer* m_vertex_buffer;
	vertex_index_buffer* m_index_buffer;
};