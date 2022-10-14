#include "mesh.h"

namespace Renderer {

    bool Mesh::CopyAssetData(const Asset::Mesh& mesh)
    {
        for (const auto& element : mesh.vertices)
        {
	        Vertex vertex;
	        vertex.pos = element.position;
            vertex.normal = element.normal;
            vertex.uv = element.uv;
            vertices.push_back(vertex);
        }
        indices = mesh.indices;
        return true;
    }
}
