#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include "Context.h"
#include <vulkan.h>
#include <memory>
#include <string>


namespace Renderer
{
	static bool GLFW_IsInit = false;

	struct WindowProperties
	{
		std::string title;
		unsigned int Width;
		unsigned int Height;
		WindowProperties(const std::string& title = "Ouroboros", uint32_t width = 1600, uint32_t height = 900) : title(title), Width(width), Height(height) {}
	};

	class Window
	{
		struct WindowData;
	public:
		Window() = delete;
		Window(const WindowProperties& window_properties);
	

		WindowData GetWindowData() { return window_data; }
		unsigned int GetWidth() { return window_data.width; }
		unsigned int GetHeight() { return window_data.height; }
		void Update();
		void Shutdown();

	private:
		void Init(const WindowProperties& window_properties);

		struct WindowData
		{
			GLFWwindow* window = nullptr;
			std::string title;
			unsigned int width = std::numeric_limits<unsigned int>::max();
			unsigned int height = std::numeric_limits<unsigned int>::max();
			std::shared_ptr<Context> RenderContextData;
		};
		WindowData window_data;
	};



}

#endif // !WINDOW_H