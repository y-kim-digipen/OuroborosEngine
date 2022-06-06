#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_map>
#include <memory>

#include <glm.hpp>
#include <glm/matrix.hpp>


#include "mesh.h"
#include "mesh_manager.h"
#include "shader_manager.h"
#include "material_manager.h"

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

	protected:
		GLFWwindow* window;
		
		GlobalData global_data;
		std::unique_ptr<UniformBuffer> global_ubo;
		std::unique_ptr<MaterialMananger> material_manager;
	};


}

#endif // !CONTEXT_H