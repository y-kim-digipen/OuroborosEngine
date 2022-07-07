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
			Script* CreateScript(std::string&& script_name, ScriptType type, std::string&& script_path = std::string());
			Script* CreateScript(const std::string& script_name, ScriptType type, const std::string& script_path = std::string());

			template<typename ... Args>
			void Update(ScriptType type, Args&& ... args);

			Script* GetScript(ScriptType type, std::string&& script_name);
			Script* GetScript(ScriptType type, const std::string& script_name);

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

		template <typename ... Args>
		void LuaScriptManager::Update(ScriptType type, Args&&... args)
		{
			for (auto& script : scripts[GetScriptTypeIdx(type)] | std::views::values)
			{
				script.Update(std::move(args)...);
			}
		}


		void TestLua();
	}
}

