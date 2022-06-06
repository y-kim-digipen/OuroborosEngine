#include "engine.h"
#include <chrono>
namespace OE
{
	void Engine::DeltaTime::Init()
	{
		start = std::chrono::system_clock::now();
		end = start;
	}

	void Engine::DeltaTime::PreUpdate()
	{
		start = std::chrono::system_clock::now();
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

		end = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> sleep_time = end - start;

		dt = sleep_time.count() * 0.001;
	}

	double Engine::DeltaTime::GetDeltaTime()
	{
		return dt;
	}
}
