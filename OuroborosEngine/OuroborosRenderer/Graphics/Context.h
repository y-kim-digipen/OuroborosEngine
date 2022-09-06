#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_map>
#include <memory>

#include <glm.hpp>
#include <map>
#include <queue>
#include <glm/matrix.hpp>


#include "mesh.h"
#include "mesh_manager.h"
#include "shader_manager.h"
#include "vulkan/vulkan_material_manager.h"
#include "texture_manager.h"
#include "../common/assets.h"
#include "../../../src/engine/ecs/components.h"

struct MaterialComponent;
static bool is_vulkan = true;
struct GLFWwindow;

namespace Renderer
{
	struct ShaderConfig;
	class Shader;
	class VulkanUniformBuffer;
	
	constexpr uint32_t max_num_lights = 20;

	struct GlobalData {
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec3 position;
	};

	struct LightGlobalData {
		Asset::LightData lights[max_num_lights];
		uint32_t num_of_lights;
	};

	class Context
	{
	public:
		Context(GLFWwindow* glfwwindow);
		virtual ~Context();

		virtual void Init(int major, int minor) = 0;

		// create descriptor set for global ubo
		virtual void InitGlobalData();
		virtual void UpdateGlobalData();
		virtual void BindGlobalData() = 0;

		void SwapBuffer();
		virtual void Shutdown() = 0;
		virtual int BeginFrame();
		virtual int EndFrame();
		
		struct DrawData
		{
			TransformComponent* transform;
			MaterialComponent* material;
			MeshComponent* mesh;
			ShaderComponent* shader;
		};

		void AddDrawQueue(TransformComponent* transform, MaterialComponent* material, MeshComponent* mesh, ShaderComponent* shader);
		virtual void DrawQueue() {};
		Asset::CameraData global_data;
		LightGlobalData light_data;
		std::unique_ptr<VulkanShaderManager> shader_manager;
		std::unique_ptr<VulkanMaterialManager> material_manager;
		std::unique_ptr<TextureManager> texture_manager_;
		
		int AddLight(uint32_t entity_id, Asset::LightData* light_component);
		void RemoveLight(uint32_t entity_id);
		void UpdateLight(uint32_t entity_id, Asset::LightData* light_component);

	protected:
		std::queue<DrawData> draw_queue;
		GLFWwindow* window;
		
		// global data ( scene data )
		VulkanUniformBuffer* global_ubo;

		// key : entity id
		// value : array index (in global_data)
		std::unordered_map<uint32_t, uint32_t> light_map;
	};


}

#endif // !CONTEXT_H