#pragma once
#include <chrono>
#include <iostream>
#include <thread>
#include <Windows.h>

#include "engine_settings.h"
#include "Graphics/shader_manager.h"
#include "Graphics/vulkan/vulkan_context.h"
#include "Graphics/camera.h"
#include "Graphics/vulkan/vulkan_material.h"

#include "Graphics/Window.h"
#include "Graphics/shader.h"
#include "input/InputManager.h"
#include "scripting/lua_script_manager.h"

#include "serializer/scene_serializer.h"

//#include "debug/profiler.h"

#ifndef ENGINE
#define ENGINE
#endif

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
		static void ShutDown() { /*Profiler::Exit();*/ }

		static auto GetGLFWWindow()
		{
			return window->GetWindowData().window;
		}

		static auto& GetECSManager()
		{
			return ecs_manager;
		}

		class DeltaTime
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
		};
		inline static DeltaTime delta_timer;

	private:
		inline static int target_fps;
		inline static double target_dt;

		

		//Modules
		inline static Input input;
	private:
		//todo to be removed
		inline static Renderer::Camera camera;

		static void SetupGUI();
		static void ECS_TestSetup();
		static void SetupModule();

	public: // Modules
		inline static Asset_Manager asset_manager;
		inline static std::unique_ptr<Renderer::Window> window;
		static void GLFW_Keyboard_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLFW_MouseButton_Callback(GLFWwindow* window, int key, int action, int mods);
		inline static ECS_Manager ecs_manager;
		inline static Script::LuaScriptManager lua_script_manager;
		inline static SceneSerializer scene_serializer;
	};

	inline Engine& Engine::Get()
	{
		static Engine* instance = new Engine();
		//OE_Assert(instance, "Engine does not properly initialized");
		return *instance;
	}
}
