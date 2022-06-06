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
	
	struct GlobalData {
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec3 position;
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
		GlobalData global_data;
		std::unique_ptr<MaterialMananger> material_manager;

	protected:
		std::queue<DrawData> draw_queue;
		GLFWwindow* window;
		
		std::unique_ptr<UniformBuffer> global_ubo;
	};


}

#endif // !CONTEXT_H