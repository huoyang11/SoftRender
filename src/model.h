#pragma once

#include <string>
#include <vector>

#include "mesh.h"

class model {
public:
	model(const std::string &p_path);
	~model();

	inline const std::vector<mesh*>& get_meshes() const { return m_meshes; };

	const std::vector<std::string>& get_material_names() const { return m_material_names; };

	model() = delete;
private:

	std::string path;

	std::vector<mesh*> m_meshes;
	std::vector<std::string> m_material_names;
};