#pragma once
//#include <array>
#include <vector>
#include "../custom_assert.h"
#include "brigand/adapted.hpp"
#include "brigand/algorithms.hpp"

namespace OE
{
	template<class> struct TupleOfVectors;

	template<template<typename ...> class T, typename ...TS>
	struct TupleOfVectors<T<TS...>>
	{
		using type = std::tuple<std::vector<TS>...>;
	};

	template<typename TSettings>
	class ComponentManager
	{
		using Settings = TSettings;

		using ComponentList = typename Settings::component_list;

	public:
		template <typename TL>
		using ComponentVectors = typename TupleOfVectors<ComponentList>::type;
		 
	public:
		static ComponentManager& Get()
		{
			static ComponentManager* instance = new ComponentManager();
			OE_Assert(instance, "Engine does not properly initialized");
			return *instance;
		}

		//bool Defragmentation();
			
		template<typename T>
		auto& GetComponent(uint64_t idx) noexcept
		{
			auto& vec = GetComponentVector<T>();
			T& val = vec.at(idx);
			return val;
		}

		void GrowCapacity(uint64_t new_capacity) noexcept
		{
			using component_list = typename Settings::component_list;
			brigand::for_each<decltype(component_vectors)>
			(
				[this, new_capacity](auto s)
				{
					using TComponent = typename decltype(s)::type;
					std::cout << typeid(TComponent).name() << std::endl;
					std::get<TComponent>(component_vectors).resize(new_capacity);
				}
			);
		}

	public:
		template<typename ComponentType>
		auto& GetComponentVector() noexcept
		{
			using ComponentVectorType = std::vector<ComponentType>;
			return std::get<ComponentVectorType>(component_vectors);
		}
	public:
		ComponentVectors<ComponentList> component_vectors;
	};
}
