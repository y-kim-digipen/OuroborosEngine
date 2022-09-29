#pragma once
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#define SOL_PRINT_ERRORS 1
#define SOL_NO_EXCEPTIONS 1

#ifdef _WIN32
#pragma comment(lib, "external/lib/lua-5.4.2/liblua54.a")
#endif

#include <sol/sol.hpp>
#include <string>
#include "../../src/engine/common.h"

namespace OE
{
	namespace Script
	{
		enum class ScriptType
		{
			Normal, Component, System, AttatchedComponent, Count
		};

		class Script
		{
		public:
			enum class State
			{
				Unloaded, Init, Update, Cleanup, Invalid
			};

			Script() = default;
			explicit Script(sol::state&& state);
			Script(sol::state&& state, std::string&& script_path);
			Script(sol::state&& state, const std::string& script_path);

			bool RunScript();
			void ChangeScript(const std::string& new_script_path);
			bool Cleanup();
			auto GetVariable(std::string var_name);

			State GetState();
			const std::string& GetUsingScriptPath();
			const char* GetLastError() const;

			sol::protected_function GetFunction(std::string func_name);

			template<typename ... Args>
			void Update(Status status, Args&& ... args);

			std::string script_path;
			sol::state sol_state;
			std::string error_what;

			State state = State::Unloaded;
		};

		inline void Script::ChangeScript(const std::string& new_script_path)
		{
			if(!new_script_path.empty())
			{
				script_path = new_script_path;
			}
			else
			{
				script_path.clear();
			}

			state = State::Unloaded;
		}

		inline bool Script::Cleanup()
		{
			if (state == State::Init || state == State::Update)
			{
				state = State::Cleanup;
				return true;
			}
			return false;
		}

		inline auto Script::GetVariable(std::string var_name)
		{
			return sol_state[var_name];
		}

		inline Script::State Script::GetState()
		{
			return state;
		}

		inline const std::string& Script::GetUsingScriptPath()
		{
			return script_path;
		}

		inline const char* Script::GetLastError() const
		{
			return error_what.c_str();
		}

		inline sol::protected_function Script::GetFunction(std::string func_name)
		{
			return sol_state[func_name];
		}

		template <typename ...Args>
		void Script::Update(Status status, Args&&... args)
		{
			switch (state)
			{
			case State::Unloaded:
			{
				if(script_path.empty())
				{
					break;
				}
				if (bool res = RunScript())
				{
					state = State::Init;
					break;
				}
				state = State::Invalid;
				break;
			}
			case State::Init:
			{
				sol::protected_function func = GetFunction("Init");
				sol::protected_function_result res = func(std::forward<Args>(args)...);
				if (res.valid() == false)
				{
					state = State::Invalid;
					sol::error err = func(args...);
					error_what = err.what();
					std::cerr << error_what << std::endl;
					break;
				}
				state = State::Update;
				break;
			}
			case State::Update:
			{
				sol::protected_function func = GetFunction("Update");
				sol::protected_function_result res = func(std::forward<Args>(args)...);
				if (res.valid() == false)
				{
					state = State::Invalid;
					sol::error err = func(args...);
					error_what = err.what();
					break;
				}
				break;
			}
			case State::Cleanup:
			{
				sol::protected_function func = GetFunction("Cleanup");
				sol::protected_function_result res = func(std::forward<Args>(args)...);
				if (res.valid() == false)
				{
					state = State::Invalid;
					sol::error err = func(args...);
					error_what = err.what();
					break;
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}
	}
}
