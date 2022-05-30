#pragma once
#include "custom_assert.h"
#include "boost/mpl/vector.hpp"

namespace OE
{
	class Engine
	{
	public:
		static Engine& Get();

		void Init();
		void PreUpdate(double [[maybe_unused]] dt) {}
		void Update(double [[maybe_unused]] dt) {}
		void PostUpdate(double [[maybe_unused]] dt) {}

		void CleanUp(){}
		void ShutDown(){}
	};

	inline Engine& Engine::Get()
	{
		static Engine* instance = new Engine();
		OE_Assert(instance, "Engine does not properly initialized");
		return *instance;
	}

	inline void Engine::Init()
	{
		//Init engine & modules
		Engine& engine = Get();
		
	}
}
