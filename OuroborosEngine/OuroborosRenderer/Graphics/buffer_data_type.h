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
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	struct LightData
	{
		glm::vec3 position;
		float cutoff;
		glm::vec3 diffuse;
		float out_cutoff;
		glm::vec3 ambient;
		float falloff;
		glm::vec3 specular;
		float padding4;
		glm::vec3 direction;
		int light_type;
	};

}
#endif //!BUFFER_DATA_TYPE_h
