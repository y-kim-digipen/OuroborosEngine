#ifndef BUFFER_DATA_TYPE_H
#define BUFFER_DATA_TYPE_H
#include <glm.hpp>

namespace Renderer
{
	struct CameraData
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec3 position;
	};

	struct MaterialData
	{
		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;
		float shininess;
	};

}
#endif //!BUFFER_DATA_TYPE_h
