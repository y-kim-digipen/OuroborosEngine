#pragma once
#include <chrono>
#include <iostream>
#include <thread>
#include <Windows.h>

namespace OE
{
	class Engine
	{
	public:
		static Engine& Get();

		static void Init()
		{
			target_fps = 240;
			target_dt = 1.0f / (target_fps * 2);

			Engine& engine = Get();
			delta_timer.Init();
		}
		static void PreUpdate()
		{
			delta_timer.PreUpdate();
		}
		static void Update()
		{

		}
		static void PostUpdate()
		{
			delta_timer.PostUpdate();
		}

		static void CleanUp(){}
		static void ShutDown(){}

		static class DeltaTime
		{
			using time_point = std::chrono::time_point<std::chrono::system_clock>;
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
	};

	inline Engine& Engine::Get()
	{
		static Engine* instance = new Engine();
		//OE_Assert(instance, "Engine does not properly initialized");
		return *instance;
	}
}
