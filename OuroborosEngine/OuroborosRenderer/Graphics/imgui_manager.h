#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H
#include <GLFW/glfw3.h>

namespace Renderer
{
	class ImguiManager
	{
	public:
		ImguiManager() = default;
		virtual void Init(GLFWwindow* window) =0;
		virtual void Update() =0;
		virtual void Shutdown() =0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
	protected:
		GLFWwindow* window_ = nullptr;
	};





}





#endif