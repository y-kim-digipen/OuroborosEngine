#include "engine.h"
#include <chrono>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <gtc/matrix_transform.hpp>

#include "engine_settings.h"
#include "../gui/viewport.h"
#include "../input/InputManager.h"

namespace OE
{
	void Engine::ECS_TestSetup()
	{
		ecs_manager.system_storage.RegisterSystemImpl<DrawSystem>([](OE::Status status, OE::ecs_ID ent, float dt, TransformComponent& transform, ShaderComponent& shader, MaterialComponent& material, MeshComponent& mesh)
			{
				const bool has_shader = Engine::asset_manager.GetManager<ShaderAssetManager>().HasAsset(shader.name);
				if(has_shader == false)
				{
					std::cerr << "From draw system : shader " + shader.name + " does not exist, draw call ignored" << std::endl;
					shader.name = "";
					return;
				}
				if(shader.name.empty())
				{
					std::cerr << "From draw system : mesh " + mesh.mesh_name + " does not exist, using default mesh" << std::endl;
					mesh.mesh_name = "model/default_cube.obj";
					return;
				}
				const bool has_mesh = Engine::asset_manager.GetManager<MeshAssetManager>().HasAsset(mesh.mesh_name);
				auto* context = dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get());
				if (ecs_manager.GetEntity(ent).alive)
				{
					CameraComponent* main_camera = GetActiveCamera();
					if(main_camera == nullptr)
					{
						auto ents = ecs_manager.GetEntitiesMatching<CameraTransformSyncSignature>();
						if(ents.empty())
						{
							auto ent = ecs_manager.CreateEntity();
							ecs_manager.AddComponent<CameraComponent>(ent.myID);
							ecs_manager.AddComponent<TransformComponent>(ent.myID);
							ecs_manager.GetComponent<CameraComponent>(ent.myID).SetUsing(true);
						}
						else
						{
							ecs_manager.GetComponent<CameraComponent>(ents.front()).SetUsing(true);
						}
						main_camera = GetActiveCamera();
						/*return;*/
					}
					Asset::CameraData camera_data;
					camera_data.position = ecs_manager.GetComponent<TransformComponent>(ecs_manager.GetComponentOwner(main_camera)).GetPosition();
					camera_data.view = main_camera->GetViewMatrix();
					camera_data.inv_view = glm::inverse(glm::transpose(camera_data.view));
					camera_data.projection = main_camera->GetPerspectiveMatrix();

					glm::vec4 world_pos = glm::vec4(0.0f, 0.0f, 4.9f, 1.0f);
					world_pos = camera_data.view* world_pos;
					world_pos = camera_data.projection * world_pos;
					world_pos /= world_pos.w;

					glm::vec4 world_pos2 = glm::vec4(0.0f, 0.0f, -45.0f, 1.0f);
					world_pos2 = camera_data.view * world_pos2;
					world_pos2 = camera_data.projection * world_pos2;
					world_pos2 /= world_pos2.w;

					glm::vec4 view_pos = glm::vec4(0.0f, 0.0f, -50.f, 1.0f);
					view_pos = camera_data.projection * view_pos;
					view_pos /= view_pos.w;

					view_pos = glm::vec4(0.0f, 0.0f, -0.1f, 1.0f);
					view_pos = camera_data.projection * view_pos;
					view_pos /= view_pos.w;

					//TODO: pass renderer camera data
					context->global_data = camera_data;

					context->UpdateGlobalData();
					context->AddDrawQueue(&transform, &material, &mesh , &shader);
				}
			});

		ecs_manager.system_storage.RegisterSystemImpl<LightSystem>([](OE::Status status, OE::ecs_ID ent, float dt, ShaderComponent& shader, LightComponent& light, TransformComponent& transform, MaterialComponent& material)
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
					light.data.pos = transform.GetPosition();
					context->UpdateLight(ent, &light.data);
				}
			});

		ecs_manager.system_storage.RegisterSystemImpl<ScriptingSystem>([](OE::Status status, OE::ecs_ID ent, float dt, ScriptComponent& script_component)
			{
				if (ecs_manager.GetEntity(ent).alive)
				{
					auto script = OE::Engine::lua_script_manager.GetScript(OE::Script::ScriptType::AttatchedComponent, std::to_string(ent));
					if (script == nullptr)
					{
						script = OE::Engine::lua_script_manager.CreateScript(std::to_string(ent), OE::Script::ScriptType::AttatchedComponent);
						script->ChangeScript(script_component.name);
					}

					if (script->script_path.empty() == false)
					{
						script->Update(status, ent, dt);

						if (script->GetState() == Script::Script::State::Invalid)
						{
							printf(" [ScriptError] From %s \t %s", script->script_path.c_str(), script->GetLastError());
							return;
						}
					}
				}
			});
	}

	void Engine::InitEssentialAssets()
	{
		asset_manager.GetManager<MeshAssetManager>().LoadAsset("model/default_cube.obj");
		asset_manager.GetManager<ImageAssetManager>().LoadAsset("images/null.png");
		asset_manager.GetManager<ShaderAssetManager>().LoadAsset("shader_lightpass");
		auto* context_data = (window->GetWindowData().RenderContextData.get());
		context_data->material_manager->SetNoneTexture(context_data->texture_manager->GetTexture("images/null.png"));
		context_data->material_manager->AddMaterial("material", Asset::MaterialData());

		ecs_manager.CreateEntity();
		auto& entity = ecs_manager.CreateEntity();
		entity.hierarchy.SetParent(0);
		auto& component = ecs_manager.AddComponent<TransformComponent>(entity.myID);
		auto owner = ecs_manager.GetComponentOwner(&component);
	}

	void Engine::Init()
	{
		Engine& engine = Get();
		scene_serializer.Init();
		ECS_TestSetup();
		
		//Init window
		window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Ouroboros Project"));
		Renderer::VulkanContext::ChangeSceneScreenSize(1600, 900);
		window->Init();
		OE::Input::Init();
		window->vulkan_imgui_manager.Init(GetGLFWWindow());



		window->GetWindowData().RenderContextData->InitGlobalData();

		//init engine module
		delta_timer.Init();

		InitEssentialAssets();


		window->GetWindowData().RenderContextData->InitGlobalData();

		
	}

	void Engine::PreUpdate()
	{
		glfwPollEvents();
		delta_timer.PreUpdate();

		ecs_manager.ForEntitiesMatching<CameraTransformSyncSignature>(0.f, [](OE::Status status, auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] CameraComponent& camera)
		{
			camera.SyncWithTransformComponent();
		});


		//ecs_manager.ForEntitiesMatching<HierarchicalTransformSignature>(0.f, [](OE::Status status, auto& ent, float dt, [[maybe_unused]] TransformComponent& transform)
		//	{
		//		decltype(ecs_manager)::Entity& entity = ecs_manager.GetEntity(ent);
		//		if (const bool is_parent = entity.hierarchy.HasParent() == false)
		//		{
		//			glm::mat4 identity{ 1.f };
		//			HierarchicalParentMatrixSet(ent, identity);
		//		}
		//	});

		const auto matching_entities = ecs_manager.GetEntitiesMatching<CameraTransformSyncSignature>();
		if(matching_entities.empty())
		{
			const auto entity = ecs_manager.CreateEntity().myID;
			ecs_manager.AddComponent<TransformComponent>(entity);
			ecs_manager.GetComponent<TransformComponent>(entity).SetPosition(glm::vec3{ 0.f, 0.f, 5.f });
			ecs_manager.AddComponent<TagComponent>(entity, "Main Camera");
			auto& camera = ecs_manager.AddComponent<CameraComponent>(entity);
			camera.SetUsing(true);
		}
		else if(matching_entities.size() > 1)
		{
			std::vector<ecs_ID> using_camera_entities;
			using_camera_entities.reserve(matching_entities.size());
			ecs_manager.ForEntitiesMatching<CameraTransformSyncSignature>(0.f, [&using_camera_entities](OE::Status, auto& ent, float, [[maybe_unused]] TransformComponent&, CameraComponent& camera)
				{
					if(camera.IsUsing())
					{
						using_camera_entities.push_back(ent);
					}
				});

			const size_t using_camera_entity_count = using_camera_entities.size();
			ecs_manager.GetComponent<CameraComponent>(using_camera_entities.front()).SetUsing(true);
			for (size_t idx = 1; idx < using_camera_entity_count; ++idx)
			{
				ecs_manager.GetComponent<CameraComponent>(using_camera_entities[idx]).SetUsing(false);
			}
		}

		while(!event_functions[EventFunctionType::PRE].empty())
		{
			auto& fn = event_functions[EventFunctionType::PRE].front();
			fn();
			event_functions[EventFunctionType::PRE].pop();
		}

		auto& scripts_set = lua_script_manager.GetScripts(Script::ScriptType::Normal);
		for (auto& script : scripts_set)
		{
			script.second.Update(Status::INIT,delta_timer.GetDeltaTime());
		}
	}

	namespace _impl
	{
		template<typename... Args>
		struct Mem_Fn
		{
			static constexpr auto Get(/*M T<Args...>::* pm*/)
			{
				return std::mem_fn(&Script::Script::Update<OE::Status, Args...>);
			}
		};

		template <typename... T>
		using mem_fn_wrapper = Mem_Fn<T...>;

		template <typename L>
		using as_mem_fn = brigand::wrap<L, mem_fn_wrapper>;
	}

	void Engine::Update()
	{
		//todo Move this into camera system and component
		if(Input::GetMouseButton(GLFW_MOUSE_BUTTON_2).IsDown())
		{
			constexpr float movement_speed = 10;
			constexpr float mouse_sensitivity = 10;
			constexpr float fast_move_factor = 2.f;
			const float dt = DeltaTime::GetDeltaTime();
			float move_velocity = movement_speed * dt;
			const float mouse_move_velocity = mouse_sensitivity * dt;
			if(Input::GetKeyboardButton(GLFW_KEY_LEFT_SHIFT).IsDown())
			{
				move_velocity *= fast_move_factor;
			}

			//if (Input::GetKeyboardButton(GLFW_KEY_W).IsDown())
			//{
			//	Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::FORWARD, move_velocity);
			//}
			//if (Input::GetKeyboardButton(GLFW_KEY_S).IsDown())
			//{
			//	Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::BACKWARD, move_velocity);
			//}
			//if (Input::GetKeyboardButton(GLFW_KEY_A).IsDown())
			//{
			//	Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::LEFT, move_velocity);
			//}
			//if (Input::GetKeyboardButton(GLFW_KEY_D).IsDown())
			//{
			//	Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::RIGHT, move_velocity);
			//}

			//const glm::ivec2 mouse_move = Input::GetMouse().GetCursorMove();
			//Engine::camera.MouseInput(-mouse_move.x, -mouse_move.y, mouse_move_velocity);
		}
		
		float dt = OE::Engine::DeltaTime::GetDeltaTime();

		using system_storage = ECS_Manager::SystemStorage;
		using system_usage_type = system_storage::system_usage_type;
		using system_list = ECS_Manager::settings::system_list;
		brigand::for_each<system_list>([dt](auto type)
			{
				using TSystem = typename decltype(type)::type;
				using function_signature = typename TSystem::function_signature;
				system_usage_type usage = system_storage::GetSystemUsage<TSystem>();
				switch (usage)
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
					if (script_path.empty())
					{
						break;
					}

					Script::Script* script = lua_script_manager.GetScript(Script::ScriptType::System, script_path);
					if (script)
					{
						using member_function = _impl::as_mem_fn<function_signature>;
						auto func = member_function::Get();
					}
					break;
				}
				}

			});

		while (!event_functions[EventFunctionType::ONUPDATE].empty())
		{
			auto& fn = event_functions[EventFunctionType::ONUPDATE].front();
			fn();
			event_functions[EventFunctionType::ONUPDATE].pop();
		}
	}

	void Engine::PostUpdate()
	{
		while (!event_functions[EventFunctionType::POST].empty())
		{
			auto& fn = event_functions[EventFunctionType::POST].front();
			fn();
			event_functions[EventFunctionType::POST].pop();
		}

		window->BeginFrame();
		window->Update();
		gui_manager.Update();
		while (!event_functions[EventFunctionType::START_OF_RENDERER_CONTEXT].empty())
		{
			auto& fn = event_functions[EventFunctionType::START_OF_RENDERER_CONTEXT].front();
			fn();
			event_functions[EventFunctionType::START_OF_RENDERER_CONTEXT].pop();
		}
		while (!event_functions[EventFunctionType::END_OF_RENDERER_END_FRAME].empty())
		{
			auto& fn = event_functions[EventFunctionType::END_OF_RENDERER_END_FRAME].front();
			fn();
			event_functions[EventFunctionType::END_OF_RENDERER_END_FRAME].pop();
		}
		while (!event_functions[EventFunctionType::END_OF_RENDERER_CONTEXT].empty())
		{
			auto& fn = event_functions[EventFunctionType::END_OF_RENDERER_CONTEXT].front();
			fn();
			event_functions[EventFunctionType::END_OF_RENDERER_CONTEXT].pop();
		}

		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->DrawQueue();

		window->EndFrame();
		Input::Update();
		delta_timer.PostUpdate();
	}

	void Engine::ChangeWindowSize(uint16_t width, uint16_t height)
	{
		GUI::ViewPort::SetTargetRenderSize(width, height);
		Engine::ecs_manager.ForEntitiesMatching<CameraTransformSyncSignature>(0.f, [width, height](OE::Status status, auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] CameraComponent& camera)
			{
				camera.SetCameraSize(width, height);
			});
		RegisterEvent(EventFunctionType::END_OF_RENDERER_END_FRAME, std::bind(&Renderer::VulkanContext::ChangeSceneScreenSize, width, height));
		//Renderer::VulkanContext::ChangeSceneScreenSize(width, height);
	}

	CameraComponent* Engine::GetActiveCamera()
	{
		ecs_ID main_camera_owner = -1;
		Engine::ecs_manager.ForEntitiesMatching<CameraTransformSyncSignature>(0.f,
			[&main_camera_owner](OE::Status, OE::ecs_ID ent, float, TransformComponent&, CameraComponent& camera)
			{
				if (camera.IsUsing())
				{
					main_camera_owner = ent;
				}
			});
		if(main_camera_owner == -1)
		{
			return nullptr;
		}
		return &Engine::ecs_manager.GetComponent<CameraComponent>(main_camera_owner);
	}

	void Engine::DeltaTime::Init()
	{
		start = std::chrono::steady_clock::now();
		end = start;
	}

	void Engine::DeltaTime::PreUpdate()
	{
		start = std::chrono::steady_clock::now();
	}

	void Engine::DeltaTime::PostUpdate()
	{
		std::chrono::duration<double, std::milli> work_time = start - end;
		end = std::chrono::steady_clock::now();
		const std::chrono::duration delta_tick = end - start;
		dt = std::chrono::duration<double>(delta_tick).count();

	}

	float Engine::DeltaTime::GetDeltaTime()
	{
		return dt;
	}

}
