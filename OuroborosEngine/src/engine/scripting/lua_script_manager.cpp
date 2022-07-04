#include "lua_script_manager.h"
#include "../engine_settings.h"
#include "../engine.h"

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
				"GetVelocityComponent"		, &ECS_Manager::GetComponent<VelocityComponent>,
				"GetLifetimeComponent"		, &ECS_Manager::GetComponent<LifeTimeComponent>,
				"GetMeshComponent"			, &ECS_Manager::GetComponent<MeshComponent>, 
				"GetBoolWrapperComponent"	, &ECS_Manager::GetComponent<BoolWrapperComponent>,
				"GetShaderComponent"		, &ECS_Manager::GetComponent<ShaderComponent>,
				"GetMaterialComponent"		, &ECS_Manager::GetComponent<MaterialComponent>,
				"GetTagComponent"			, &ECS_Manager::GetComponent<TagComponent>,
				"GetLightComponent"			, &ECS_Manager::GetComponent<LightComponent>,	
				
				"HasTransformComponent"		, &ECS_Manager::HasComponent<TransformComponent>,
				"HasVelocityComponent"		, &ECS_Manager::HasComponent<VelocityComponent>,
				"HasLifetimeComponent"		, &ECS_Manager::HasComponent<LifeTimeComponent>,
				"HasMeshComponent"			, &ECS_Manager::HasComponent<MeshComponent>,
				"HasBoolWrapperComponent"	, &ECS_Manager::HasComponent<BoolWrapperComponent>,
				"HasShaderComponent"		, &ECS_Manager::HasComponent<ShaderComponent>,
				"HasMaterialComponent"		, &ECS_Manager::HasComponent<MaterialComponent>,
				"HasTagComponent"			, &ECS_Manager::HasComponent<TagComponent>,
				"HasLightComponent"			, &ECS_Manager::HasComponent<LightComponent>,
				
				"HasPhysicsSystem"			, &ECS_Manager::HasSystem<PhysicsSystem>,
				"HasTestSystem"				, &ECS_Manager::HasSystem<TestSystem>,
				"HasDrawSystem"				, &ECS_Manager::HasSystem<DrawSystem>,
				"HasLightSystem"			, &ECS_Manager::HasSystem<LightSystem>,
				"HasScriptingSystem"		, &ECS_Manager::HasSystem<ScriptingSystem>
			);

			state.new_usertype<OE::Engine::DeltaTime>("DeltaTimer"
				, "dt", &OE::Engine::DeltaTime::GetDeltaTime);

			//Components
			state.new_usertype<TransformComponent>("TransformComponent",
				sol::constructors<TransformComponent()>()
				, "pos", &TransformComponent::pos
				, "scale", &TransformComponent::scale
				, "rotation", &TransformComponent::rotation
				);

			state.new_usertype<VelocityComponent>("VelocityComponent",
				sol::constructors<VelocityComponent()>()
				, "vel", &VelocityComponent::vel
				);

			state.new_usertype<LifeTimeComponent>("LifeTimeComponent",
				sol::constructors<LifeTimeComponent()>()
				, "life_time", &LifeTimeComponent::life_time
				);

			state.new_usertype<MeshComponent>("MeshComponent",
				sol::constructors<MeshComponent()>()
				, "mesh_name", &MeshComponent::mesh_name
				);

			state.new_usertype<Asset::MaterialData>("MaterialData",
				sol::constructors<Asset::MaterialData()>()
				, "ambient", &Asset::MaterialData::ambient
				, "diffuse", &Asset::MaterialData::diffuse
				, "specular", &Asset::MaterialData::specular
				, "shininess", &Asset::MaterialData::shininess
				);

			state.new_usertype<MaterialComponent>("MaterialComponent",
				sol::constructors<MaterialComponent()>()
				, "name", &MaterialComponent::name
				, "data", &MaterialComponent::data
				, "flag", &MaterialComponent::flag
				, "is_light", &MaterialComponent::is_light
				);

			state.new_usertype<ShaderComponent>("ShaderComponent",
				sol::constructors<ShaderComponent()>()
				, "name", &ShaderComponent::name
				);

			state.new_usertype<BoolWrapperComponent>("BoolWrapperComponent",
				sol::constructors<BoolWrapperComponent()>()
				, "bool_type", &BoolWrapperComponent::bool_type
				);

			state.new_usertype<TagComponent>("TagComponent",
				sol::constructors<TagComponent()>()
				, "tag", &TagComponent::tag
				);

			state.new_usertype<Asset::LightData>("LightData",
			    sol::constructors<Asset::LightData()>()
			    , "position", &Asset::LightData::position
			    , "diffuse", &Asset::LightData::diffuse
				, "ambient", &Asset::LightData::ambient
				, "specular", &Asset::LightData::specular
				, "direction", &Asset::LightData::direction
			);

			state.new_usertype<LightComponent>("LightComponent",
				sol::constructors<LightComponent()>()
				, "init", &LightComponent::init
				, "data", &LightComponent::data
				);

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