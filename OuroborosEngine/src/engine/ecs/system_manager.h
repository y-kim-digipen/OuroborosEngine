#pragma once
#include <brigand/adapted.hpp>

template<class> struct TupleOfFunctions;
//
//template<typename ...TS>
//struct TupleOfFunctions
//{
//	using type = std::tuple<typename TS::function_type...>;
//};

template<template<typename ...> class T, typename ...TS>
struct TupleOfFunctions<T<TS...>>
{
	using type = std::tuple<typename TS::function_type...>;
};

template<typename TSystemList>
class SystemManager
{
public:
	using system_list = TSystemList;
	using type = SystemManager <TSystemList>;

	using SystemImplContainer = typename TupleOfFunctions<system_list>::type;

public:
	template<typename TSystem>
	constexpr void RegisterSystemImpl(typename TSystem::function_type&& function)
	{
		auto& function_impl = std::get<TSystem::function_type>(system_impl_container);
		OE_Assert(function_impl == nullptr, "Trying to register system already implemented");
		function_impl = std::move(function);
	}

	template<typename TSystem>
	constexpr void RegisterSystemImpl(typename TSystem::function_type& function)
	{
		auto& function_impl = std::get<TSystem::function_type>(system_impl_container);
		OE_Assert(function_impl == nullptr, "Trying to register system already implemented");
		function_impl = std::move(function);
	}

	template<typename TSystem>
	auto GetSystemImplementation() const
	{
		return std::get<TSystem::function_type>(system_impl_container);
	}

	template<typename TSystem>
	auto& GetSystemImplementation()
	{
		return std::get<TSystem::function_type>(system_impl_container);
	}

	template<typename TSystem>
	bool HasSystemImplementation() const
	{
		return GetSystemImplementation<TSystem>() != nullptr;
	}
private:
	SystemImplContainer system_impl_container;
};