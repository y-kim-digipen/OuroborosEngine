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

#include "../../../src/engine/ecs/components.h"
static bool is_vulkan = true;
struct GLFWwindow;

namespace Renderer
{
	struct ShaderConfig;
	class Shader;
	
	struct GlobalData {
		glm::mat4 projection;
		glm::mat4 view;
	};

	class Context
	{
	public:
		Context(GLFWwindow* glfwwindow);
		virtual ~Context();

		virtual void Init(int major, int minor) = 0;

		// create descriptor set for global ubo
		virtual void InitGlobalData() = 0;

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


	protected:
		std::queue<DrawData> draw_queue;
		GLFWwindow* window;
		GlobalData global_data;
	};


}

#endif // !CONTEXT_H