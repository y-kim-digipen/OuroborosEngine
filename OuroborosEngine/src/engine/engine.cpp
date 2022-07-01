#include "engine.h"
#include <chrono>
#include <gtc/matrix_transform.hpp>

#include "engine_settings.h"
#include "gui/gui_component_panel.h"


namespace OE
{
	void Engine::SetupGUI()
	{
		window->vulkan_imgui_manager.RegisterPanel("File", "FileBrowser", &OE::FileSystemPanelFunction, false);
		window->vulkan_imgui_manager.RegisterPanel("System", "AssetManager", &OE::AssetBrowserPanelFunction, false);
		window->vulkan_imgui_manager.RegisterPanel("System", "EngineInfo", &OE::EngineInfoPanelFunction, false);
		window->vulkan_imgui_manager.RegisterPanel("ECS", "Entities", &OE::EntityInfoPanelFunction);
		window->vulkan_imgui_manager.RegisterPanel("ECS", "SystemInfo", &OE::SystemInfoPanelFunction);
		//window->vulkan_imgui_manager.RegisterPanel("System", "Scripts", &OE::SystemInfoPanelFunction);
	}

	void Engine::ECS_TestSetup()
	{
		auto& ent = ecs_manager.CreateEntity();
		ecs_manager.AddComponent<TransformComponent>(ent.myID, glm::vec3{ 0.f, 1.f, 0.f });
		ecs_manager.AddComponent<VelocityComponent>(ent.myID, glm::vec3{ 2.f, 3.f, 0.f });
		auto& ent2 = ecs_manager.CreateEntity();
		ecs_manager.AddComponent<LifeTimeComponent>(ent2.myID, 5);
		ecs_manager.AddComponent<MeshComponent>(ent2.myID);
		auto& ent3 = ecs_manager.CreateEntity();
		ecs_manager.AddComponent<BoolWrapperComponent>(ent3.myID, false);

		auto& ent4 = ecs_manager.CreateEntity();
		ecs_manager.AddComponent<TransformComponent>(ent4.myID, glm::vec3{ 4.f, 5.f, 0.f });
		ecs_manager.AddComponent<VelocityComponent>(ent4.myID, glm::vec3{ 6.f, 7.f, 0.f });
		ecs_manager.AddComponent<TagComponent>(ent4.myID, "TestEntity");

		ecs_manager.ForEntitiesMatching<PhysicsSystem>(1.2f, [](auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
			{
				std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
				std::cerr << "Transform: " << transform.pos.x << ", " << transform.pos.y << std::endl;
				std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
			});

		ecs_manager.ForEntitiesMatching<Signature0>(1.2f, [](auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
			{
				std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
				std::cerr << "Transform: " << transform.pos.x << ", " << transform.pos.y << std::endl;
				std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
			});

		ecs_manager.system_storage.RegisterSystemImpl<PhysicsSystem>([](OE::ecs_ID ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
			{
				transform.pos += velocity.vel * dt;
			});
		ecs_manager.system_storage.RegisterSystemImpl<DrawSystem>([](OE::ecs_ID ent, float dt, TransformComponent& transform, ShaderComponent& shader, MaterialComponent& material, MeshComponent& mesh)
			{
				auto* context = dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get());
				if (ecs_manager.GetEntity(ent).alive)
				{
					camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
					camera.data.view = camera.GetCameraMat();

					//TODO: pass renderer camera data
					context->global_data = camera.data;
					context->UpdateGlobalData();
			
					context->AddDrawQueue(&transform, &material, &mesh, &shader);
				}
			});

		ecs_manager.system_storage.RegisterSystemImpl<LightSystem>([](OE::ecs_ID ent, float dt, ShaderComponent& shader, LightComponent& light, TransformComponent& transform, MaterialComponent& material)
			{
				auto* context = dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get());
				if(ecs_manager.GetEntity(ent).alive)
				{
					if(light.init == false)
					{
						context->AddLight(ent, &light.data);
						material.is_light = true;
						light.init = true;
					}
					light.data.position = transform.pos;
					material.data.diffuse = light.data.diffuse;
					context->UpdateLight(ent, &light.data);
				}
			});

		ecs_manager.system_storage.RegisterSystemImpl<ScriptingSystem>([](OE::ecs_ID ent, float dt, ScriptComponent& script_component)
			{
				if (ecs_manager.GetEntity(ent).alive)
				{
					auto script = OE::Engine::lua_script_manager.GetScript(OE::Script::ScriptType::Component, script_component.name);
					script->Update(ent, dt);

					if(script->GetState() == Script::Script::State::Invalid)
					{
						assert(false);
					}
				}
			});
	}

	void Engine::SetupModule()
	{
		asset_manager.GetManager<MeshAssetManager>().LoadAsset("suzanne.obj");
	}

	void Engine::Init()
	{
		/*
		target_fps = 240;
		target_dt = 1.0f / (target_fps * 2);*/
		//Init engine
		Engine& engine = Get();

		ECS_TestSetup();

		//Init window
		window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Ouroboros Project"));
		camera.data.projection = glm::perspective(glm::radians(90.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
		camera.data.view = camera.GetCameraMat();



		Renderer::ShaderConfig shader_config
		{		"shader",
		{	Renderer::E_StageType::VERTEX_SHADER,
					Renderer::E_StageType::FRAGMENT_SHADER },
			2 };

		Renderer::ShaderConfig shader_config2{
					"shader2",
			{	Renderer::E_StageType::VERTEX_SHADER,
						Renderer::E_StageType::FRAGMENT_SHADER	},2 };


		Renderer::ShaderConfig shader_config3{
					"light_shader",
			{	Renderer::E_StageType::VERTEX_SHADER,
						Renderer::E_StageType::FRAGMENT_SHADER	},2 };

		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->shader_manager_.AddShader(&shader_config);
		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->shader_manager_.AddShader(&shader_config2);
		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->shader_manager_.AddShader(&shader_config3);

		window->GetWindowData().RenderContextData->InitGlobalData();

		//init engine module
		SetupGUI();
		delta_timer.Init();

		std::vector<int> key_supports{
			GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, 
			GLFW_KEY_SPACE
		};
		input.Init(window->GetWindowData().window, key_supports);
		input.RegisterCallback(GLFW_KEY_SPACE, [](Input::Modes mode)
			{
				if (mode == Input::PRESSED)
				{
					std::cout << "Pressed" << std::endl;
				}
				else
				{
					std::cout << "Released" << std::endl;
				}
			});

		
		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->material_manager->AddMaterial("material", Asset::MaterialData());
		SetupModule();
	}

	void Engine::PreUpdate()
	{
		delta_timer.PreUpdate();
		
	}

	namespace _impl
	{

		//template <typename R>
		//class MultiFunc
		//{
		//	typedef void(*function_t)();
		//	function_t m_func;
		//public:
		//	template <typename ...A1>
		//	MultiFunc<R>(R(*f)(A1...))
		//	{
		//		m_func = (void(*)())f;
		//	}

		//	template <typename ...A1>
		//	MultiFunc<R> operator =(R(*f)(A1...))
		//	{
		//		m_func = (void(*)())f;
		//		return *this;
		//	}


		//	template <typename ...A1>
		//	R operator()(A1... a1) const
		//	{
		//		R(*f)(A1...) = (R(*)(A1...))(m_func);
		//		return (*f)(a1...);
		//	}
		//};

		////X = parent class
		////R = ReturnType
		//template <typename X, typename R>
		//class MultiFunc2
		//{
		//	typedef void(X::* function_t)();

		//	function_t m_func;
		//	X* m_obj;
		//public:

		//	template <typename ...A1>
		//	MultiFunc2<X, R>(X* obj, R(X::* f)(A1...))
		//	{
		//		m_func = (void(X::*)())(f);
		//		m_obj = obj;
		//	}


		//	template <typename ...A1>
		//	MultiFunc2<X, R> operator =(R(X::* f)(A1...))
		//	{
		//		m_func = (void(X::*)())(f);
		//		return *this;
		//	}


		//	template <typename ...A1>
		//	R operator()(A1&&... a1) const
		//	{
		//		std::cout << sizeof...(A1) << std::endl;
		//		R(X:: * fn_ptr)(A1...) = (R(X::*)(A1...))(m_func);

		//		return ((*m_obj).*fn_ptr)(std::forward<A1>(a1)...);
		//	}
		//};

		//template <typename X, typename R>
		//class MultiFunc3
		//{
		//	typedef void(X::* function_t)();

		//	function_t m_func;
		//	X* m_obj;
		//public:

		//	template <typename ...A1>
		//	MultiFunc3<X, R>(X* obj, R(X::* f)(A1...))
		//	{
		//		m_func = (void(X::*)())(f);
		//		m_obj = obj;
		//	}


		//	template <typename ...A1>
		//	MultiFunc3<X, R> operator =(R(X::* f)(A1...))
		//	{
		//		m_func = (void(X::*)())(f);
		//		return *this;
		//	}


		//	template <typename ...A1>
		//	R operator()(A1&&... a1) const
		//	{
		//		std::cout << sizeof...(A1) << std::endl;
		//		R(X:: * fn_ptr)(A1...) = (R(X::*)(A1...))(m_func);

		//		return ((*m_obj).*fn_ptr)(std::forward<A1>(a1)...);
		//	}
		//};

		//template <typename... T>
		//using expand_script_call_wrapper = MultiFunc3<Script::Script, void>;

		//template <typename L>
		//using as_expand_script_call = brigand::wrap<L, expand_script_call_wrapper>;

		template<typename... Args>
		struct Mem_Fn
		{
			//template< class M, template<typename ...> class T >
			static constexpr auto Get(/*M T<Args...>::* pm*/)
			{
				return std::mem_fn(&Script::Script::Update<Args...>);
				//auto f = static_cast<M  T<typename Args...>::*>(pm);
				//return std::mem_fn(pm);
			}
		};

		template <typename... T>
		using mem_fn_wrapper = Mem_Fn<T...>;

		template <typename L>
		using as_mem_fn = brigand::wrap<L, mem_fn_wrapper>;

	}

	void Engine::Update()
	{
		input.Update();
		float dt = OE::Engine::DeltaTime::GetDeltaTime();

		using system_storage = ECS_Manager::SystemStorage;
		using system_usage_type = system_storage::system_usage_type;
		using system_list = ECS_Manager::settings::system_list;
		brigand::for_each<system_list>([dt](auto type)
			{
				using TSystem = typename decltype(type)::type;
				using function_signature = typename TSystem::function_signature;
				system_usage_type usage = system_storage::GetSystemUsage<TSystem>();
				switch(usage)
				{
				case system_usage_type::NONE:
					{
					break;
					}
				case system_usage_type::NATIVE:
					{
					ecs_manager.UpdateNativeSystem<TSystem>(dt);
					break;
					}
				case system_usage_type::SCRIPT:
					{
					const std::string script_path = system_storage::GetSystemScript<TSystem>();
					if(script_path.empty())
					{
						break;
					}

					Script::Script* script  = lua_script_manager.GetScript(Script::ScriptType::System, script_path);
					if(script)
					{
						//_impl::MultiFunc2<Script::Script, void> caller(script, &Script::Script::Update<ecs_ID, float, BoolWrapperComponent&>);
				/*		using Functor = _impl::as_expand_script_call<typename TSystem::required_components>;
						Fu*/
						using member_function = _impl::as_mem_fn<function_signature>;
						auto func = member_function::Get(/*&Script::Script::Update*/);
						//auto func = std::bind(bind, *script);
						ecs_manager.ForEntitiesMatching<TSystem>(dt, script, func);
					}
					break;
					}
				}
				
			});
	}

	void Engine::PostUpdate()
	{
		window->BeginFrame();
		window->Update();
		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->DrawQueue();
		window->EndFrame();


		delta_timer.PostUpdate();
	}

	void Engine::DeltaTime::Init()
	{
		start = std::chrono::steady_clock::now();
		end = start;
	}

	void Engine::DeltaTime::PreUpdate()
	{
		start = std::chrono::steady_clock::now();
		auto& scripts = lua_script_manager.GetScripts(Script::ScriptType::Normal);
		for (auto& script : scripts)
		{
			script.second.Update(GetDeltaTime());
		}
	}

	void Engine::DeltaTime::PostUpdate()
	{

		std::chrono::duration<double, std::milli> work_time = start - end;

		//if (work_time.count() < target_dt * 1000)
		//{
		//	std::chrono::duration<double, std::milli> delta_ms(target_dt * 1000 - work_time.count());
		//	auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
		//	std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		//}


		end = std::chrono::steady_clock::now();
		const std::chrono::duration delta_tick = end - start;
		dt = std::chrono::duration<double>(delta_tick).count();

	}

	double Engine::DeltaTime::GetDeltaTime()
	{
		return dt;
	}

}
