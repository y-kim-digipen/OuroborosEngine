#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include "../../engine/engine.h"

namespace OE
{
	class Input
	{
	public:
		enum Modes
		{
			PRESSED, RELEASED
		};

		static void Init(GLFWwindow* window, std::vector<int> support_keys)
		{
			glfwSetKeyCallback(window, KeyboardCallback);
			glfwSetMouseButtonCallback(window, MouseButtonCallback);

			for (auto element : support_keys)
			{
				keys[element] = false;
			}
		}

		static void Update()
		{
			old_keys = keys;
		}

		static bool Pressed(int key)
		{
			if (!HasKey(key))
			{
				return false;
			}
			return keys[key] && !old_keys[key];
		}

		static bool Released(int key)
		{
			if (!HasKey(key))
			{
				return false;
			}
			return !keys[key] && old_keys[key];
		}

		static bool Down(int key)
		{
			if (!HasKey(key))
			{
				return false;
			}
			return keys[key];
		}

		static void RegisterCallback(int key, std::function<void(Modes)>&& function, bool wake_on_register = true)
		{
			key_callbacks[key] = std::make_pair(wake_on_register, std::move(function));
		}

		static void RemoveCallback(int key)
		{
			key_callbacks.erase(key_callbacks.find(key));
		}

	private:
		static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (key_callbacks.contains(key) && key_callbacks[key].first)
			{
				key_callbacks[key].second(action == GLFW_PRESS ? PRESSED : RELEASED);
			}
			if (!HasKey(key))
			{
				return;
			}
			action == GLFW_PRESS ? keys[key] = true : keys[key] = false;
		}

		static void MouseButtonCallback(GLFWwindow* window, int key, int action, int mods)
		{
			if (key_callbacks.contains(key) && key_callbacks[key].first)
			{
				key_callbacks[key].second(action == GLFW_PRESS ? PRESSED : RELEASED);
			}
			if (!HasKey(key))
			{
				return;
			}
			action == GLFW_PRESS ? keys[key] = true : keys[key] = false;
		}

		static bool HasKey(int key)
		{
			return keys.contains(key);
		}

		static inline std::map<int, bool> old_keys;
		static inline std::map<int, bool> keys;

		static inline std::map<int, std::pair<bool, std::function<void(Modes)>>> key_callbacks;
	};
}
