#pragma once

#define NOMINMAX
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <detail/type_vec.hpp>

#include <glm/glm.hpp>

//#include <assimp/scene.h>
//#include <assimp/cimport.h>
//#include <assimp/postprocess.h>

//#include <assimp/material.h>
//#include <assimp/mesh.h>

class aiScene;
class aiMesh;
class aiMaterial;

namespace yk_ModelLoader
{
	struct MeshEntryPoint
	{
		MeshEntryPoint() : _name{}, _numVertices{ 0 }, _numIndices{ 0 }, _vertexOffset{ 0 }, _indexOffset{ 0 }, _materialIndex{ 0 } {}
		std::string _name;
		uint32_t _numVertices;
		uint32_t _numIndices;
		uint32_t _vertexOffset;
		uint32_t _indexOffset;
		uint32_t _materialIndex;
	};

	struct Mesh
	{
		std::string					_name;
		std::vector<glm::vec3>		_vertexPositions;
		std::vector<glm::vec3>		_vertexNormals;
		std::vector<glm::vec3>		_vertexTangents;
		std::vector<glm::vec3>		_vertexBitangents;
		std::vector<glm::vec2>		_vertexTexCoords;
		std::vector<uint32_t>		_indices;

		std::vector<MeshEntryPoint> _meshEntryPoints;
	};

	struct Texture
	{
		Texture() : _width{ 0 }, _height{ 0 } {}
		std::string					_name;
		uint32_t					_width, _height;
	};
}
namespace yk_ModelLoader
{
	class Mesh;
	struct ModelLoadResult
	{
		ModelLoadResult() :
			_result{ Result::INVALID }, _numMeshes{ 0 }, _mesh{ nullptr }, _numVertices{ 0 }, _numIndices{ 0 },
			_isUsingNormal{ false }, _isUsingUV{ false }, _isUsingTangent{ false } {}
		enum class Result { INVALID, SUCCESS, FILE_READ_ERROR, MESH_READ_ERROR, };
		Result					_result;
		uint32_t				_numMeshes;
		Mesh* _mesh;

		uint32_t				_numVertices;
		uint32_t				_numIndices;

		bool					_isUsingNormal;
		bool					_isUsingUV;
		bool					_isUsingTangent;

		std::vector<Texture>	_usingTextureSet;
	};

	class ModelLoader
	{
	public:
		static bool Import(const char* path, ModelLoadResult* result);
	private:
		static bool ProcessMeshes(const aiScene* scene, ModelLoadResult* result);
		static bool ProcessSingleMesh(const aiMesh* mesh, ModelLoadResult* result, uint32_t meshIndex, uint32_t prevIndexOffset);
		static void ReserveMeshSpace(const aiScene* scene, ModelLoadResult* result);

		static bool ProcessTextures(const aiScene* scene, ModelLoadResult* result);
		static bool ProcessMaterials(const aiScene* scene, ModelLoadResult* result);
		static bool ProcessSingleMaterial(const aiMaterial* material, ModelLoadResult* result);
	private:
		static const aiScene* s_processingScene;
	};
}

