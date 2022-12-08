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
			
			uint32_t index0 = face.mIndices[0] + prevIndexOffset;
			uint32_t index1 = face.mIndices[1] + prevIndexOffset;
			uint32_t index2 = face.mIndices[2] + prevIndexOffset;

			// Tangent vector creation code
			glm::vec3 v0 = processingMesh._vertexPositions[index0];
			glm::vec3 v1 = processingMesh._vertexPositions[index1];
			glm::vec3 v2 = processingMesh._vertexPositions[index2];

			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;

			float deltaU1 = processingMesh._vertexTexCoords[index1].x - processingMesh._vertexTexCoords[index0].x;
			float deltaV1 = processingMesh._vertexTexCoords[index1].y - processingMesh._vertexTexCoords[index0].y;

			float deltaU2 = processingMesh._vertexTexCoords[index2].x - processingMesh._vertexTexCoords[index0].x;
			float deltaV2 = processingMesh._vertexTexCoords[index2].y - processingMesh._vertexTexCoords[index0].y;

			float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
			float fc = 1.0f / dividend;

			glm::vec3 tangent = {
				(fc * (deltaV2 * edge1.x - deltaV1 * edge2.x)),
				(fc * (deltaV2 * edge1.y - deltaV1 * edge2.y)),
				(fc * (deltaV2 * edge1.z - deltaV1 * edge2.z))
			};

			tangent = glm::normalize(tangent);
			
			//float sx = deltaU1, sy = deltaU2;
			//float tx = deltaV1, ty = deltaV2;
			//float handedness = ((tx * sy - ty * sx) < 0.0f) ? -1.0f : 1.0f;
			processingMesh._vertexTangents[index0] = tangent;
			processingMesh._vertexTangents[index1] = tangent;
			processingMesh._vertexTangents[index2] = tangent;
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
		if (result->_isUsingUV)
			result->_mesh->_vertexTexCoords.resize(numVertices);
			
		result->_mesh->_vertexTangents.resize(numVertices);
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

