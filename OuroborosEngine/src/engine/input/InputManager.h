#pragma once
#include <vector>
#include <GLFW/glfw3.h>
class InputManager
{
	void Init(std::vector<int> support_keys)
	{
		glfwSetKeyCallback(window, Callback);
	}



private:
	static void Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};