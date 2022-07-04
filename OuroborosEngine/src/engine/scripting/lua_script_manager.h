#pragma once
#include "script.h"
#include <map>
#include <ranges>
namespace OE
{
	namespace Script
	{
		class LuaScriptManager
		{
		public:
			Script* CreateScript(std::string&& script_name, ScriptType type, std::string&& script_path = std::string())
			{
				sol::state sol_state = Create(sol::state());
				auto res = scripts[GetScriptTypeIdx(type)].try_emplace(script_name, std::move(sol_state));
				if (res.second)
				{
					if(script_path.empty() == false)
					{
						res.first->second.ChangeScript(script_path);
					}
					return &res.first->second;
				}
				return nullptr;
			}

			Script* CreateScript(const std::string& script_name, ScriptType type, const std::string& script_path = std::string())
			{
				sol::state sol_state = Create(sol::state());
				auto res = scripts[GetScriptTypeIdx(type)].try_emplace(script_name, std::move(sol_state));
				if (res.second)
				{
					if (script_path.empty() == false)
					{
						res.first->second.ChangeScript(script_path);
					}
					return &res.first->second;
				}
				return nullptr;
			}

			template<typename ... Args>
			void Update(ScriptType type, Args&& ... args)
			{
				for (auto& script : scripts[GetScriptTypeIdx(type)] | std::views::values)
				{
					script.Update(std::move(args)...);
				}
			}

			Script* GetScript(ScriptType type, std::string&& script_name)
			{
				bool contains = scripts[GetScriptTypeIdx(type)].contains(script_name);
				if (contains == false)
					return nullptr;
				return &scripts[GetScriptTypeIdx(type)][script_name];
			}

			Script* GetScript(ScriptType type, const std::string& script_name)
			{
				bool contains = scripts[GetScriptTypeIdx(type)].contains(script_name);
				if (contains == false)
					return nullptr;
				return &scripts[GetScriptTypeIdx(type)][script_name];
			}

			auto& GetScripts(ScriptType type)
			{
				return scripts[GetScriptTypeIdx(type)];
			}
		private:
			static size_t GetScriptTypeIdx(ScriptType type)
			{
				return static_cast<size_t>(type);
			}

			static sol::state Create(sol::state&& sol_state)
			{
				sol_state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::math);
				RegisterUserDateTypes(sol_state);
				RegisterGlobalFunction(sol_state);
				return sol_state;
			}

			static void RegisterUserDateTypes(sol::state& state);
			static void RegisterGlobalFunction(sol::state& state);
		public:
			std::array<std::map<std::string, Script>, static_cast<size_t>(ScriptType::Count)> scripts;
		};

		void TestLua();
	}
}

