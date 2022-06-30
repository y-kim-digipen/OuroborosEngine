#include "lua_script_manager.h"
#include "../engine_settings.h"
#include "../engine.h"

auto& GetECSManager()
{
	return OE::Engine::Get().ecs_manager;
}

using AsOverload = brigand::list<>;

Script::Script(sol::state&& state, std::string&& script_path) : state(std::move(state)), script_path(script_path)
{}

Script::Script(sol::state&& state, const std::string& script_path) : state(std::move(state)), script_path(script_path)
{
}

void Script::RunScript()
{
	//lua_close(state);
	state.script_file(script_path);
}

void LuaScriptManager::RegisterUserDateTypes(sol::state& state)
{		//Math types
	state.new_usertype<glm::vec3>("vec3",
		sol::constructors<glm::vec3(), glm::vec3(glm::vec3), glm::vec3(float), glm::vec3(float, float, float)>()
		, "x", &glm::vec3::x
		, "y", &glm::vec3::y
		, "z", &glm::vec3::z);

	//Engine
	state.set("GetECS_Manager", &GetECSManager);
	state.new_usertype<ECS_Manager>("ECS_Manager"
		, "GetTransformComponent", &ECS_Manager::GetComponent<TransformComponent>
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, VelocityComponent>,
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, LifeTimeComponent>,
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, MeshComponent>, 
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, BoolWrapperComponent>,
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, ShaderComponent>,
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, MaterialComponent>,
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, TagComponent>,
											//&ECS_Manager::SOL_GetComponent<OE::ecs_ID, LightComponent>	
									
		, "HasTransformComponent", &ECS_Manager::HasComponent<TransformComponent>
											//&ECS_Manager::HasComponent<VelocityComponent>,
											//&ECS_Manager::HasComponent<LifeTimeComponent>,
											//&ECS_Manager::HasComponent<MeshComponent>,
											//&ECS_Manager::HasComponent<BoolWrapperComponent>,
											//&ECS_Manager::HasComponent<ShaderComponent>,
											//&ECS_Manager::HasComponent<MaterialComponent>,
											//&ECS_Manager::HasComponent<TagComponent>,
											//&ECS_Manager::HasComponent<LightComponent>
										//)

		//, "HasSystem", sol::overload(		&ECS_Manager::HasSystem<TransformComponent>,
		//									&ECS_Manager::HasSystem<VelocityComponent>,
		//									&ECS_Manager::HasSystem<LifeTimeComponent>,
		//									&ECS_Manager::HasSystem<MeshComponent>,
		//									&ECS_Manager::HasSystem<BoolWrapperComponent>,
		//									&ECS_Manager::HasSystem<ShaderComponent>,
		//									&ECS_Manager::HasSystem<MaterialComponent>,
		//									&ECS_Manager::HasSystem<TagComponent>,
		//									&ECS_Manager::HasSystem<LightComponent>
		//							)
									
		//, "HasSystem", sol::overload(		&ECS_Manager::HasSystem<TransformComponent>,
		//									&ECS_Manager::HasSystem<VelocityComponent>,
		//									&ECS_Manager::HasSystem<LifeTimeComponent>,
		//									&ECS_Manager::HasSystem<MeshComponent>,
		//									&ECS_Manager::HasSystem<BoolWrapperComponent>,
		//									&ECS_Manager::HasSystem<ShaderComponent>,
		//									&ECS_Manager::HasSystem<MaterialComponent>,
		//									&ECS_Manager::HasSystem<TagComponent>,
		//									&ECS_Manager::HasSystem<LightComponent>
		//							)
		);

	state.new_usertype<OE::Engine::DeltaTime>("DeltaTimer"
		, "dt", &OE::Engine::DeltaTime::GetDeltaTime);

	//Components
	state.new_usertype<TransformComponent>("TransformComponent",
		sol::constructors<TransformComponent()>()
		, "pos", &TransformComponent::pos
		, "scale", &TransformComponent::scale
		, "rotate_axis", &TransformComponent::rotate_axis
		, "angle", &TransformComponent::angle
		);

	state.new_usertype<BoolWrapperComponent>("BoolWrapperComponent",
		sol::constructors<BoolWrapperComponent()>()
		, "bool_type", &BoolWrapperComponent::bool_type
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
	Script& lua = OE::Engine::Get().lua_script_manager.CreateScript("scripts/test.lua", Script::Type::Component);
	lua.RunScript();
	//std::string var = lua.GetVariable("some_str");
	//lua.script_file("scripts/test.lua");
	//lua.script("print('bark bark bark!')");
}
