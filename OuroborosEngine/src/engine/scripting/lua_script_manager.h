#pragma once

#include <map>
#include <ranges>
#include <brigand/algorithms.hpp>


extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "external/lib/lua-5.4.2/liblua54.a")
#endif

#include <sol/sol.hpp>

class Script
{
public:
	enum struct Type
	{
		Normal, Component, System
	};

	Script(){}
	Script(sol::state&& state, std::string&& script_path);
	Script(sol::state&& state, const std::string& script_path);
	void RunScript();
	auto GetVariable(std::string var_name)
	{
		return state[var_name];
	}
	sol::protected_function GetFunction(std::string func_name)
	{
		return state[func_name];
	}

	sol::state state;
	std::string script_path;
};

class LuaScriptManager
{
public:
	Script& CreateScript(std::string&& script_path, Script::Type type)
	{
		auto script_to_create = Script(sol::state(), std::move(script_path));
		SetUp(script_to_create);
		scripts[type].push_back(Script(std::move(script_to_create)));
		return scripts[type].back();
	}

	Script& CreateScript(const std::string& script_path, Script::Type type)
	{
		auto script_to_create = Script(sol::state(), script_path);
		SetUp(script_to_create);
		scripts[type].push_back(Script(std::move(script_to_create)));
		return scripts[type].back();
	}

	void Update(float dt)
	{
		for (auto& script : scripts[Script::Type::Normal])
		{
			auto res = script.GetFunction("Update")(dt);
		}
	}

	Script* GetScript(Script::Type type, std::string&& script_path)
	{
		for (auto& script : scripts[type])
		{
			if(script.script_path == script_path)
			{
				return &script;
			}
		}
		return nullptr;
	}

	Script* GetScript(Script::Type type, const std::string& script_path)
	{
		for (auto& script : scripts[type])
		{
			if (script.script_path == script_path)
			{
				return &script;
			}
		}
		return nullptr;
	}

	bool ReloadScript(const std::string & script_path)
	{
		std::string name = script_path;
		Script::Type type_to_reload = Script::Type::Normal;

		std::string str_path_to_reload = script_path;

		for (auto& [type, scripts] : scripts)
		{
			//auto itr_to_delete = std::find(scripts.begin(), scripts.end(), [str_path_to_reload](Script& val)
			//	{
			//		return val.script_path == str_path_to_reload;
			//		//return true;
			//	});
			auto itr_to_delete = scripts.begin();
			if(itr_to_delete != scripts.end())
			{
				scripts.erase(itr_to_delete);
				type_to_reload = type;
				break;
			}
		}

		CreateScript(script_path, type_to_reload);
		return true;
	}

private:
	static void SetUp(Script& script)
	{
		script.state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::math);
		RegisterUserDateTypes(script.state);
		RegisterGlobalFunction(script.state);

		script.RunScript();
		auto init_func = script.GetFunction("Init");
		auto res = init_func();
	}

	static void RegisterUserDateTypes(sol::state& state);
	static void RegisterGlobalFunction(sol::state& state);

public:
	std::unordered_map<Script::Type, std::vector<Script>> scripts;
};

void TestLua();

