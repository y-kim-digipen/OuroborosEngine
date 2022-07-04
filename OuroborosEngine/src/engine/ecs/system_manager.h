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

	constexpr static auto num_systems = std::tuple_size_v<SystemImplContainer>;

	enum class system_usage_type
	{
		NONE, NATIVE, SCRIPT
	};

public:
	template<typename TSystem>
	constexpr void RegisterSystemImpl(typename TSystem::function_type&& function, bool run_native = true)
	{
		auto& function_impl = std::get<TSystem::function_type>(system_impl_container);
		OE_Assert(function_impl == nullptr, "Trying to register system already implemented");
		function_impl = std::move(function);
		if(run_native)
			system_usage[brigand::index_of<system_list, TSystem>{}] = system_usage_type::NATIVE;
	}

	template<typename TSystem>
	constexpr void RegisterSystemImpl(typename TSystem::function_type& function, bool run_native = true)
	{
		auto& function_impl = std::get<TSystem::function_type>(system_impl_container);
		OE_Assert(function_impl == nullptr, "Trying to register system already implemented");
		function_impl = std::move(function);
		if (run_native)
			system_usage[brigand::index_of<system_list, TSystem>{}] = system_usage_type::NATIVE;
	}

	template<typename TSystem>
	constexpr static void RegisterSystemScript(const std::string& script_path)
	{
		size_t system_index = brigand::index_of<system_list, TSystem>{};
		////auto& function_impl = std::get<TSystem::function_type>(system_impl_container);
		////OE_Assert(function_impl == nullptr, "Trying to register system already implemented");
		//bool is_script_valid = OE::Engine::
		//if()
		system_scripts_container[system_index] = script_path;
	}

	template<typename TSystem>
	constexpr static void RegisterSystemScript(std::string&& script_path)
	{
		size_t system_index = brigand::index_of<system_list, TSystem>{};
		////auto& function_impl = std::get<TSystem::function_type>(system_impl_container);
		////OE_Assert(function_impl == nullptr, "Trying to register system already implemented");
		//bool is_script_valid = OE::Engine::
		//if()
		system_scripts_container[system_index] = std::move(script_path);
	}

	template<typename TSystem>
	auto static GetSystemScript()
	{
		size_t system_index = brigand::index_of<system_list, TSystem>{};
		return system_scripts_container[system_index];
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
	bool HasSystemImplementation()
	{
		return GetSystemImplementation<TSystem>() != nullptr;
	}

	template<typename TSystem>
	bool HasSystemScript()
	{
		return !GetSystemScript<TSystem>().empty();
	}

	template<typename TSystem>
	static void SetSystemUsage(system_usage_type usage)
	{
		size_t system_index = brigand::index_of<system_list, TSystem>{};
		system_usage[system_index] = usage;
	}

	template<typename TSystem>
	static system_usage_type GetSystemUsage() 
	{
		size_t system_index = brigand::index_of<system_list, TSystem>{};
		return system_usage[system_index];
	}
private:
	SystemImplContainer system_impl_container;
	static inline std::array<system_usage_type, num_systems> system_usage;
	static inline std::array<std::string, num_systems> system_scripts_container;
};