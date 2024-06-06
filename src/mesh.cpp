#include "mesh.h"

mesh::mesh(const std::vector<Vertex>& p_vertices, const std::vector<uint32_t>& p_indices)
{
	uint32_t size = sizeof(Vertex) * p_vertices.size();
	void* buffer = calloc(1, size);
	memcpy(buffer, p_vertices.data(), size);
	m_vertex_buffer = new vertex_buffer(buffer, p_vertices.size());
	m_vertex_buffer->set_item_size(sizeof(Vertex));

	size = sizeof(uint32_t) * p_indices.size();
	buffer = calloc(1, size);
	memcpy(buffer, p_indices.data(), size);
	m_index_buffer = new vertex_index_buffer((uint32_t *)buffer, p_indices.size());
}

mesh::~mesh()
{
	if (m_vertex_buffer) {
		free(m_vertex_buffer->get_buffer());
		m_vertex_buffer = nullptr;
	}

	if (m_index_buffer) {
		free(m_index_buffer->get_buffer());
		m_index_buffer = nullptr;
	}
}
