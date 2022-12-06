#include "ModelLoader.h"

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

namespace yk_ModelLoader
{
	const aiScene* ModelLoader::s_processingScene = nullptr;
	bool ModelLoader::Import(const char* path, ModelLoadResult* result)
	{
		constexpr unsigned assimpImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType;

		if (result == nullptr)
		{
			//LOG("Invalid result struct given", "stop reading file", path);
			return false;
		}

		s_processingScene = aiImportFile(path, assimpImportFlags);
		if (s_processingScene == nullptr)
		{
			//LOG("Failed to importe scene file", path);
			result->_result = ModelLoadResult::Result::FILE_READ_ERROR;
			return false;
		}

		//Import scene
		//LOG("Start importing scene", path, "...");

		result->_mesh = new Mesh;
		result->_mesh->_name = s_processingScene->mName.C_Str();
		const bool meshProcessRes = ProcessMeshes(s_processingScene, result);
		if (meshProcessRes == false)
		{
			result->_result = ModelLoadResult::Result::MESH_READ_ERROR;
			s_processingScene = nullptr;
			delete result->_mesh;
			result->_mesh = nullptr;
			return false;
		}
		ProcessTextures(s_processingScene, result);
		ProcessMaterials(s_processingScene, result);
		result->_result = ModelLoadResult::Result::SUCCESS;
		return true;
	}

	bool ModelLoader::ProcessMeshes(const aiScene* scene, ModelLoadResult* result)
	{
		const uint32_t numMeshesInScene = scene->mNumMeshes;
		ReserveMeshSpace(scene, result);
		//LOG("\t", "Num meshes :", numMeshesInScene);
		result->_numMeshes = numMeshesInScene;
		uint32_t prevIndexOffset = 0;

		for (uint32_t i = 0; i < numMeshesInScene; ++i)
		{
			if (ProcessSingleMesh(scene->mMeshes[i], result, i, prevIndexOffset) == false)
			{
				return false;
			}

			prevIndexOffset += scene->mMeshes[i]->mNumVertices;
		}
		return true;
	}

	bool ModelLoader::ProcessSingleMesh(const aiMesh* mesh, ModelLoadResult* result, uint32_t meshIndex, uint32_t prevIndexOffset)
	{
		//LOG("Processing mesh :", mesh->mName.C_Str());
		const uint32_t numVertices = mesh->mNumVertices;
		const uint32_t numFaces = mesh->mNumFaces;
		const uint32_t numIndices = numFaces * 3;

		Mesh& processingMesh = *result->_mesh;
		MeshEntryPoint& processingEntryPoint = processingMesh._meshEntryPoints[meshIndex];
		processingEntryPoint._name = mesh->mName.C_Str();
		processingEntryPoint._vertexOffset = result->_numVertices;
		processingEntryPoint._indexOffset = result->_numIndices;
		processingEntryPoint._numVertices = numVertices;
		processingEntryPoint._numIndices = numIndices;


		constexpr auto aiVector3ToGlmVec3 = [](const aiVector3D& vector) -> glm::vec3
		{
			return { vector.x, vector.y, vector.z };
		};
		constexpr auto aiVector2ToGlmVec2 = [](const aiVector2D& vector) -> glm::vec2
		{
			return { vector.x, vector.y };
		};

		//Process vertex position
		if (mesh->HasPositions())
		{
			for (uint32_t i = 0; i < numVertices; ++i)
			{
				const aiVector3D& aiVertexPos = mesh->mVertices[i];
				processingMesh._vertexPositions[result->_numVertices + i] = aiVector3ToGlmVec3(aiVertexPos);
			}
		}
		else
		{
			//LOG("Failed to read mesh", "mesh does not have any vertex position");
			return false;
		}

		//Process vertex normal
		if (mesh->HasNormals())
		{
			for (uint32_t i = 0; i < numVertices; ++i)
			{
				const aiVector3D& aiVertexNormal = mesh->mNormals[i];
				processingMesh._vertexNormals[result->_numVertices + i] = aiVector3ToGlmVec3(aiVertexNormal);
			}
		}

		//Process UV coordinate
		if (mesh->HasTextureCoords(0))
		{
			for (uint32_t i = 0; i < numVertices; ++i)
			{
				const aiVector3D& aiVertexUV = mesh->mTextureCoords[0][i];
				processingMesh._vertexTexCoords[result->_numVertices + i] = aiVector2ToGlmVec2(aiVector2D{ aiVertexUV.x, -aiVertexUV.y });
			}
		}

		if (mesh->HasTangentsAndBitangents())
		{
			for (uint32_t i = 0; i < numVertices; ++i)
			{
				processingMesh._vertexTangents[result->_numVertices + i] = aiVector3ToGlmVec3(mesh->mTangents[i]);
				processingMesh._vertexBitangents[result->_numVertices + i] = aiVector3ToGlmVec3(mesh->mBitangents[i]);
			}
		}

		//Process indices
		//processingMesh._indices.resize()
		for (uint32_t i = 0; i < numFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			const uint32_t indicesInFace = face.mNumIndices;
			assert(indicesInFace == 3);
			for (uint8_t j = 0; j < indicesInFace; ++j)
			{
				processingMesh._indices[result->_numIndices + i * 3 + j] = face.mIndices[j] + prevIndexOffset;
			}
		}

		result->_numVertices += numVertices;
		result->_numIndices += numIndices;

		return true;
	}

	void ModelLoader::ReserveMeshSpace(const aiScene* scene, ModelLoadResult* result)
	{
		const uint32_t numMeshesInScene = scene->mNumMeshes;
		uint32_t numVertices = 0;
		uint32_t numIndices = 0;
		for (uint32_t i = 0; i < numMeshesInScene; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			result->_isUsingNormal |= mesh->HasNormals();
			result->_isUsingUV |= mesh->HasTextureCoords(0);
			result->_isUsingTangent |= mesh->HasTangentsAndBitangents();

			numVertices += mesh->mNumVertices;
			numIndices += mesh->mNumFaces * 3;
		}
		result->_mesh->_meshEntryPoints.resize(numMeshesInScene);

		result->_mesh->_vertexPositions.resize(numVertices);
		if (result->_isUsingNormal)
			result->_mesh->_vertexNormals.resize(numVertices);
		if(result->_isUsingTangent)
		{
			result->_mesh->_vertexTangents.resize(numVertices);
			result->_mesh->_vertexBitangents.resize(numVertices);
		}
		if (result->_isUsingUV)
			result->_mesh->_vertexTexCoords.resize(numVertices);
		//if (result->_isUsingTangent)
			result->_mesh->_indices.resize(numIndices);
	}

	bool ModelLoader::ProcessTextures(const aiScene* scene, ModelLoadResult* result)
	{
		const uint32_t numTextures = scene->mNumTextures;
		result->_usingTextureSet.resize(numTextures);
		//LOG("Number of textures this model using :", numTextures);
		for (uint32_t i = 0; i < numTextures; ++i)
		{
			const aiTexture* texture = scene->mTextures[i];
			result->_usingTextureSet[i]._name = texture->mFilename.C_Str();
			result->_usingTextureSet[i]._width = texture->mWidth;
			result->_usingTextureSet[i]._height = texture->mHeight;
			//LOG("\t", result->_usingTextureSet[i]._name, "Width :", result->_usingTextureSet[i]._width, "Height :", result->_usingTextureSet[i]._height);
		}
		return true;
	}

	bool ModelLoader::ProcessMaterials(const aiScene* scene, ModelLoadResult* result)
	{
		const uint32_t numMaterials = scene->mNumMaterials;
		for (uint32_t i = 0; i < numMaterials; ++i)
		{
			ProcessSingleMaterial(scene->mMaterials[i], result);
		}
		return true;
	}

	bool ModelLoader::ProcessSingleMaterial(const aiMaterial* material, ModelLoadResult* result)
	{
		//std::cout << material->GetName().C_Str();
		//int i = material->GetTextureCount(aiTextureType_BASE_COLOR);
		return true;
	}

}

