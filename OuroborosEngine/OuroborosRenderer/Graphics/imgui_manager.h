#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H
#include <functional>
#include <map>
#include <ranges>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui-docking/imgui.h>

namespace Renderer
{
	class ImguiManager
	{
	public:
		ImguiManager() = default;
		virtual void Init(GLFWwindow* window) = 0;
		virtual void Update();

		virtual void Shutdown() =0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
	protected:
		GLFWwindow* window_ = nullptr;
	};
}
#endif