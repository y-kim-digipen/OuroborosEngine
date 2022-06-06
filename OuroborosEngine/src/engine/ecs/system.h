#pragma once
#include <utility>
namespace OE
{
	namespace ECS
	{
		namespace _impl
		{
			template<typename ...TComponents>
			using _system_function_wrapper = std::function<void(ecs_ID, float, TComponents&...)>;

			template <typename L>
			using as_system_function = brigand::wrap<L, _system_function_wrapper>;
		}

		template<typename ...TRequiredComponents>
		struct System
		{
			using type = System<TRequiredComponents...>;
			using signature = brigand::append<brigand::list<type>, brigand::list<TRequiredComponents...>>;
			using function_type = _impl::as_system_function<brigand::list<TRequiredComponents...>>;
			using required_components = brigand::list<TRequiredComponents...>;
		};
	}
}

