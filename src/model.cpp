#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>

#include "model.h"

static void ProcessMesh(void* p_transform, struct aiMesh* p_mesh, const struct aiScene* p_scene, std::vector<Vertex>& p_outVertices, std::vector<uint32_t>& p_outIndices)
{
	aiMatrix4x4 meshTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform);

	for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
	{
		aiVector3D position = meshTransformation * p_mesh->mVertices[i];
		aiVector3D normal = meshTransformation * (p_mesh->mNormals ? p_mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f));
		aiVector3D texCoords = p_mesh->mTextureCoords[0] ? p_mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
		aiVector3D tangent = p_mesh->mTangents ? meshTransformation * p_mesh->mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);
		aiVector3D bitangent = p_mesh->mBitangents ? meshTransformation * p_mesh->mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f);

		p_outVertices.push_back
		(
			{
				position.x,
				position.y,
				position.z,
				texCoords.x,
				texCoords.y,
				normal.x,
				normal.y,
				normal.z,
				tangent.x,
				tangent.y,
				tangent.z,
				bitangent.x,
				bitangent.y,
				bitangent.z
			}
		);
	}

	for (uint32_t faceID = 0; faceID < p_mesh->mNumFaces; ++faceID)
	{
		auto& face = p_mesh->mFaces[faceID];

		for (size_t indexID = 0; indexID < 3; ++indexID)
			p_outIndices.push_back(face.mIndices[indexID]);
	}
}

static void ProcessNode(void* p_transform, struct aiNode* p_node, const struct aiScene* p_scene, std::vector<mesh*>& p_meshes)
{
	aiMatrix4x4 nodeTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform) * p_node->mTransformation;

	// Process all the node's meshes (if any)
	for (uint32_t i = 0; i < p_node->mNumMeshes; ++i)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		aiMesh* aimesh = p_scene->mMeshes[p_node->mMeshes[i]];
		ProcessMesh(&nodeTransformation, aimesh, p_scene, vertices, indices);
		p_meshes.push_back(new mesh(vertices, indices));
	}

	// Then do the same for each of its children
	for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
	{
		ProcessNode(&nodeTransformation, p_node->mChildren[i], p_scene, p_meshes);
	}
}

model::model(const std::string& p_path) : path(p_path)
{
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(p_path,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_PreTransformVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_FindInstances |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_Debone |
		aiProcess_GlobalScale
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		throw std::string("loading model error path : ") + p_path;

	for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		if (material)
		{
			aiString name;
			aiGetMaterialString(material, AI_MATKEY_NAME, &name);
			m_material_names.push_back(name.C_Str());
		}
	}

	aiMatrix4x4 identity;
	ProcessNode(&identity, scene->mRootNode, scene, m_meshes);
}

model::~model()
{
}
