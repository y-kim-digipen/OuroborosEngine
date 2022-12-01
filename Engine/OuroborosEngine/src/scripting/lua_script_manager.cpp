#include "lua_script_manager.h"
#include "../core/engine_settings.h"
#include "../core/engine.h"
#include "../../src/input/InputManager.h"
namespace OE
{
	namespace Script
	{
		auto& GetECSManager()
		{
			return OE::Engine::ecs_manager;
		}

		Script::Script(sol::state&& state) : sol_state(std::move(state)){}

		Script::Script(sol::state&& state, std::string&& script_path)
			: script_path(script_path), sol_state(std::move(state)) {}

		Script::Script(sol::state&& state, const std::string& script_path)
			: script_path(script_path), sol_state(std::move(state)) {}

		bool Script::RunScript()
		{
			auto res = sol_state.script_file(script_path);
			return res.valid();
		}

		Script* LuaScriptManager::CreateScript(std::string&& script_name, ScriptType type, std::string&& script_path)
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

		Script* LuaScriptManager::CreateScript(const std::string& script_name, ScriptType type,
			const std::string& script_path)
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

		Script* LuaScriptManager::GetScript(ScriptType type, std::string&& script_name)
		{
			bool contains = scripts[GetScriptTypeIdx(type)].contains(script_name);
			if (contains == false)
				return nullptr;
			return &scripts[GetScriptTypeIdx(type)][script_name];
		}

		Script* LuaScriptManager::GetScript(ScriptType type, const std::string& script_name)
		{
			bool contains = scripts[GetScriptTypeIdx(type)].contains(script_name);
			if (contains == false)
				return nullptr;
			return &scripts[GetScriptTypeIdx(type)][script_name];
		}

		void LuaScriptManager::DeleteScript(ScriptType type, const std::string& script_name)
		{
			bool contains = scripts[GetScriptTypeIdx(type)].contains(script_name);
			if (contains == false)
				return;
			scripts[GetScriptTypeIdx(type)].erase(scripts[GetScriptTypeIdx(type)].find(script_name));
		}

		void LuaScriptManager::DeleteScript(const std::string& script_name)
		{
			for(int i = 0; i < static_cast<int>(ScriptType::Count); ++i)
			{
				DeleteScript(static_cast<ScriptType>(i), script_name);
			}
		}

		void LuaScriptManager::RegisterUserDateTypes(sol::state& state)
		{		//Math types
			state.new_usertype<glm::vec3>("vec3",
				sol::constructors<glm::vec3(), glm::vec3(glm::vec3), glm::vec3(float), glm::vec3(float, float, float)>()
				//, sol::resolve<>
				, "x", &glm::vec3::x
				, "y", &glm::vec3::y
				, "z", &glm::vec3::z);

			//state.new_usertype<glm::vec3>("vec4",
			//	sol::constructors<glm::vec4(), glm::vec4(glm::vec4), glm::vec4(float), glm::vec4(float, float, float, float)>()
			//	, "x", &glm::vec4::x
			//	, "y", &glm::vec4::y
			//	, "z", &glm::vec4::z
			//	, "w", &glm::vec4::w);

			//Engine
			state.set("GetECS_Manager", &GetECSManager);
			state.new_usertype<ECS_Manager>("ECS_Manager",
				"GetTransformComponent"		, &ECS_Manager::GetComponent<TransformComponent>,
				"GetCameraComponent"		, &ECS_Manager::GetComponent<CameraComponent>,
				"GetMeshComponent"			, &ECS_Manager::GetComponent<MeshComponent>, 
				"GetShaderComponent"		, &ECS_Manager::GetComponent<ShaderComponent>,
				"GetMaterialComponent"		, &ECS_Manager::GetComponent<MaterialComponent>,
				"GetTagComponent"			, &ECS_Manager::GetComponent<TagComponent>,
				"GetLightComponent"			, &ECS_Manager::GetComponent<LightComponent>,
				
				"HasTransformComponent"		, &ECS_Manager::HasComponent<TransformComponent>,
				"HasCameraComponent", & ECS_Manager::HasComponent<CameraComponent>,
				"HasMeshComponent"			, &ECS_Manager::HasComponent<MeshComponent>,
				"HasShaderComponent"		, &ECS_Manager::HasComponent<ShaderComponent>,
				"HasMaterialComponent"		, &ECS_Manager::HasComponent<MaterialComponent>,
				"HasTagComponent"			, &ECS_Manager::HasComponent<TagComponent>,
				"HasLightComponent"			, &ECS_Manager::HasComponent<LightComponent>,

				"HasDrawSystem"				, &ECS_Manager::HasSystem<DrawSystem>,
				"HasLightSystem"			, &ECS_Manager::HasSystem<LightSystem>,
				"HasScriptingSystem"		, &ECS_Manager::HasSystem<ScriptingSystem>
			);

			state.new_usertype<OE::Engine::DeltaTime>("DeltaTimer"
				, "dt", &OE::Engine::DeltaTime::GetDeltaTime);

			//Components
			state.new_usertype<TransformComponent>("TransformComponent",
				sol::constructors<TransformComponent()>()
				, "GetPosition",	&TransformComponent::GetPosition
				, "GetScale",		&TransformComponent::GetScale
				, "GetRotation",	&TransformComponent::GetRotation
				, "SetPosition",	&TransformComponent::SetPosition
				, "SetScale",		&TransformComponent::SetScale
				, "SetRotation",	&TransformComponent::SetRotation

				);
			
			state.new_usertype<MeshComponent>("MeshComponent",
				sol::constructors<MeshComponent()>()
				, "mesh_name", &MeshComponent::mesh_name
				);

			state.new_usertype<CameraComponent>("MeshComponent",
				sol::constructors<CameraComponent()>()
				, "IsUsing", &CameraComponent::IsUsing
				, "SetUsing", &CameraComponent::SetUsing
				, "GetUpVector", &CameraComponent::GetUpVector
				, "GetRightVector", &CameraComponent::GetRightVector
				, "GetFrontVector", &CameraComponent::GetFrontVector
				);

			state.new_usertype<Asset::MaterialData>("MaterialData",
				sol::constructors<Asset::MaterialData()>()
				, "albedo", &Asset::MaterialData::albedo
				, "metallic", &Asset::MaterialData::metallic
				, "roughness", &Asset::MaterialData::roughness
				, "ao", &Asset::MaterialData::ao
				, "has_albedo_texture", &Asset::MaterialData::has_albedo_texture
				, "has_normal_texture", &Asset::MaterialData::has_normal_texture
				, "has_metalroughness_texture", &Asset::MaterialData::has_metalroughness_texture
				, "has_ao_texture", &Asset::MaterialData::has_ao_texture
				);

			state.new_usertype<MaterialComponent>("MaterialComponent",
				sol::constructors<MaterialComponent()>()
				, "name", &MaterialComponent::name
				, "data", &MaterialComponent::data
				, "flag", &MaterialComponent::flag
				, "is_light", &MaterialComponent::is_light
				, "texture_albedo_name", &MaterialComponent::texture_albedo_name
				, "texture_normal_name", &MaterialComponent::texture_normal_name
				, "texture_metalroughness_name", &MaterialComponent::texture_metalroughness_name
				, "texture_ao_name", &MaterialComponent::texture_ao_name
				);

			state.new_usertype<ShaderComponent>("ShaderComponent",
				sol::constructors<ShaderComponent()>()
				, "name", &ShaderComponent::name
				);

			state.new_usertype<TagComponent>("TagComponent",
				sol::constructors<TagComponent()>()
				, "tag", &TagComponent::tag
				);

			state.new_usertype<Asset::LightData>("LightData",
			    sol::constructors<Asset::LightData()>()
			    , "position", &Asset::LightData::pos
			    , "diffuse", &Asset::LightData::diffuse
				, "cutoff", &Asset::LightData::cutoff
				, "dir", &Asset::LightData::dir
				, "falloff", &Asset::LightData::falloff
				, "out_cutoff", &Asset::LightData::out_cutoff
				, "type", &Asset::LightData::type
			);

			state.new_usertype<LightComponent>("LightComponent",
				sol::constructors<LightComponent()>()
				, "init", &LightComponent::init
				, "data", &LightComponent::data
				);

			//Input

			static auto IsKeyboardButtonDown = [](int32_t key)
			{
				return OE::Input::GetKeyboardButton(key).IsDown();
			};
			static auto IsKeyboardButtonPressed = [](int32_t key)
			{
				return OE::Input::GetKeyboardButton(key).IsPressed();
			};
			static auto IsKeyboardButtonReleased= [](int32_t key)
			{
				return OE::Input::GetKeyboardButton(key).IsReleased();
			};
			static auto IsMouseButtonDown = [](int32_t key)
			{
				return OE::Input::GetMouseButton(key).IsDown();
			};
			static auto IsMouseButtonPressed = [](int32_t key)
			{
				return OE::Input::GetMouseButton(key).IsPressed();
			};
			static auto IsMouseButtonReleased = [](int32_t key)
			{
				return OE::Input::GetMouseButton(key).IsReleased();
			};
			static auto GetMouseMoveX = []()
			{
				return OE::Input::GetMouse().GetCursorMove().x;
			};
			static auto GetMouseMoveY = []()
			{
				return OE::Input::GetMouse().GetCursorMove().y;
			};
			static auto GetMouseX = []()
			{
				return OE::Input::GetMouse().GetCursorPos().x;
			};
			static auto GetMouseY = []()
			{
				return OE::Input::GetMouse().GetCursorPos().y;
			};

			state.set("IsKeyboardButtonDown",		&IsKeyboardButtonDown);
			state.set("IsKeyboardButtonReleased",	&IsKeyboardButtonPressed);
			state.set("IsKeyboardButtonPressed",	&IsKeyboardButtonReleased);
			state.set("IsMouseButtonDown",		 &IsMouseButtonDown);
			state.set("IsMouseButtonReleased	", &IsMouseButtonPressed);
			state.set("IsMouseButtonPressed"	, &IsMouseButtonReleased);
			state.set("GetMouseMoveX", &GetMouseMoveX);
			state.set("GetMouseMoveY", &GetMouseMoveY);
			state.set("GetMouseX", &GetMouseX);
			state.set("GetMouseY", &GetMouseY);
		}

		void LuaScriptManager::RegisterGlobalFunction(sol::state& state)
		{
			state.set("GetECSManager", &GetECSManager);
			auto float_sin = [](float f)
			{
				return glm::sin(f);
			};
			auto float_cos = [](float f)
			{
				return glm::cos(f);
			};
			state.set("sin", float_sin);
			state.set("cos", float_cos);
		}

		void TestLua()
		{
			//Script* script = OE::Engine::Get().lua_script_manager.CreateScript("TestScript", ScriptType::Normal, "scripts/test.lua");
			//bool res = script->RunScript();
			//if (res == false)
			//{
			//	std::cerr << "DASDAS" << std::endl;
			//}
			//script->GetVariable("Hi");
			//std::string var = lua.GetVariable("some_str");
			//lua.script_file("scripts/test.lua");
			//lua.script("print('bark bark bark!')");
		}

	}
}