#pragma once
#include <chrono>
#include <iostream>
#include <thread>
#include <Windows.h>

#include "Graphics/shader_manager.h"
#include "Graphics/vulkan/vulkan_context.h"
#include "Graphics/camera.h"
#include "Graphics/vulkan/vulkan_material.h"

#include "Graphics/Window.h"
#include "Graphics/shader.h"
#include "input/InputManager.h"

namespace OE
{

	class Engine
	{
	public:
		static Engine& Get();

		static void Init();

		static void PreUpdate();
		static void Update();
		static void PostUpdate();
		static void CleanUp(){}
		static void ShutDown(){}

		static auto GetGLFWWindow()
		{
			return window->GetWindowData().window;
		}

		static class DeltaTime
		{
			using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
		public:
			static void Init();
			static void PreUpdate();
			static void PostUpdate();
			static double GetDeltaTime();
		private:
			inline static time_point start;
			inline static time_point end;
			inline static double dt;
		} delta_timer;

	private:
		inline static int target_fps;
		inline static double target_dt;

		inline static std::unique_ptr<Renderer::Window> window;

		//Modules
		inline static Input input;
	private:
		//todo to be removed
		inline static Renderer::Camera camera;

		static void SetupGUI();
		static void ECS_TestSetup();

		static void GLFW_Keyboard_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLFW_MouseButton_Callback(GLFWwindow* window, int key, int action, int mods);
	};

	inline Engine& Engine::Get()
	{
		static Engine* instance = new Engine();
		//OE_Assert(instance, "Engine does not properly initialized");
		return *instance;
	}
}
