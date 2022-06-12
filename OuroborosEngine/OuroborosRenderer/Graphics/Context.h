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
#include "material_manager.h"

#include "../../../src/engine/ecs/components.h"

struct MaterialComponent;
static bool is_vulkan = true;
struct GLFWwindow;

namespace Renderer
{
	struct ShaderConfig;
	class Shader;
	class UniformBuffer;
	
	constexpr uint32_t max_num_lights = 20;

	struct GlobalData {
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec3 position;
	};

	struct LightData {
		LightComponent lights[max_num_lights];
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
		
		std::unordered_map<const char*, std::unique_ptr<Shader>> shader_map;
		std::unordered_map<const char*, std::unique_ptr<Mesh>> mesh_map;

		struct DrawData
		{
			TransformComponent* transform;
			MaterialComponent* material;
			MeshComponent* mesh;
			ShaderComponent* shader;
		};

		void AddDrawQueue(TransformComponent* transform, MaterialComponent* material, MeshComponent* mesh, ShaderComponent* shader);
		virtual void DrawQueue() {};
		
		// Global Data ( camera data and global lights )
		GlobalData global_data;
		LightData light_data;

		
		int AddLight(uint32_t entity_id, LightComponent* light_component);
		void RemoveLight(uint32_t entity_id);
		void UpdateLight(uint32_t entity_id, LightComponent* light_component);

		std::unique_ptr<MaterialMananger> material_manager;
	protected:
		std::queue<DrawData> draw_queue;
		GLFWwindow* window;
		
		// global data ( scene data )
		std::unique_ptr<UniformBuffer> global_ubo;
		std::unique_ptr<UniformBuffer> light_ubo;

		// key : entity id
		// value : array index (in global_data)
		std::unordered_map<uint32_t, uint32_t> light_map;
	};


}

#endif // !CONTEXT_H