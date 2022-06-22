//#pragma once
//extern "C"
//{
//#include "lua.h"
//#include "lualib.h"
//}
//
//#ifdef _WIN32
//#pragma comment(lib, "external/lib/lua-5.4.2/liblua54.a")
//#endif

#pragma once
//#include <iostream>
//
#include <rttr/registration>
#include <rttr/type>
#include "../src/engine/engine_settings.h"
//
//static void f() { std::cout << "Hello World" << std::endl; }
//
//RTTR_REGISTRATION
//{
//	rttr::registration::method("f", &f);
//}

static void Register();
static void RegisterComponentInformation();


template<>
inline void RTTRComponent<TransformComponent>()
{
	rttr::registration::class_<TransformComponent>(typeid(TransformComponent).name())
		.constructor()
		//.property("pos", &TransformComponent::pos)
		//.property("scale", &TransformComponent::scale)
		//.property("rotate_axis", &TransformComponent::rotate_axis)
		.property("angle", &TransformComponent::angle);
};


template<>
inline void RTTRComponent<LifeTimeComponent>()
{
	rttr::registration::class_<LifeTimeComponent>(typeid(LifeTimeComponent).name())
		.constructor()
		//.property("pos", &TransformComponent::pos)
		//.property("scale", &TransformComponent::scale)
		//.property("rotate_axis", &TransformComponent::rotate_axis)
		.property("life_time", &LifeTimeComponent::life_time);
};

RTTR_REGISTRATION{
	Register();
}

inline void Register()
{
	RegisterComponentInformation();
}

inline void RegisterComponentInformation()
{
	using component_list = ComponentList;
	brigand::for_each<component_list>([](auto type)
		{
			using TComponent = typename decltype(type)::type;
			RTTRComponent<TComponent>();
		});
}