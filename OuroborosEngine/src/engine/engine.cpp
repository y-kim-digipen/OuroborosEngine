#include "engine.h"
#include <chrono>
namespace OE
{
	void Engine::DeltaTime::Init()
	{
		start = std::chrono::high_resolution_clock::now();
		end = start;
	}

	void Engine::DeltaTime::PreUpdate()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	void Engine::DeltaTime::PostUpdate()
	{

		std::chrono::duration<double, std::milli> work_time = start - end;

		//if (work_time.count() < target_dt * 1000)
		//{
		//	std::chrono::duration<double, std::milli> delta_ms(target_dt * 1000 - work_time.count());
		//	auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
		//	std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		//}


		end = std::chrono::high_resolution_clock::now();
		const std::chrono::duration delta_tick = end - start;
		dt = std::chrono::duration<double>(delta_tick).count();

	}

	double Engine::DeltaTime::GetDeltaTime()
	{
		return dt;
	}
}
