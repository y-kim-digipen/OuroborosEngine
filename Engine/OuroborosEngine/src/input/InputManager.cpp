#include "InputManager.h"
#include "../core/engine.h"

namespace OE
{

	void Mouse::Update()
	{
		old_cursor_pos = current_cursor_pos;
		for (auto& mouse_button : mouse_buttons | std::views::values)
		{
			mouse_button.Update();
		}
	}

	void Input::Init()
	{
		GLFWwindow* glfw_window;
		glfw_window = OE::Engine::Get().GetGLFWWindow();
		glfwSetKeyCallback(glfw_window, GLFW_KeyboardCallback);
		glfwSetMouseButtonCallback(glfw_window, GLFW_MouseButtonCallback);
		glfwSetCursorPosCallback(glfw_window, GLFW_CursorCallback);
	}

	void Input::Update()
	{
		for (auto& keyboard_button : keyboard_buttons | std::views::values)
		{
			keyboard_button.Update();
		}
		mouse.Update();
	}

	void Input::GLFW_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			keyboard_buttons[key].current = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keyboard_buttons[key].current = false;
		}
	}

	void Input::GLFW_CursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		const glm::ivec2 new_mouse_pos = glm::ivec2{ -xpos, ypos };
		mouse.current_cursor_pos = new_mouse_pos;
	}

	void Input::GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			mouse.mouse_buttons[button].current = true;
		}
		else if (action == GLFW_RELEASE)
		{
			mouse.mouse_buttons[button].current = false;
		}
	}
}
