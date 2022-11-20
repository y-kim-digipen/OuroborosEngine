#include "engine.h"
#include <chrono>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <gtc/matrix_transform.hpp>

#include "engine_settings.h"

namespace OE
{
	void Engine::ECS_TestSetup()
	{
		ecs_manager.ForEntitiesMatching<PhysicsSystem>(1.2f, [](OE::Status status, auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
			{
				std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
				std::cerr << "Transform: " << transform.pos.x << ", " << transform.pos.y << std::endl;
				std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
			});

		ecs_manager.ForEntitiesMatching<Signature0>(1.2f, [](OE::Status status, auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
			{
				std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
				std::cerr << "Transform: " << transform.pos.x << ", " << transform.pos.y << std::endl;
				std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
			});

		ecs_manager.system_storage.RegisterSystemImpl<PhysicsSystem>([](OE::Status status, OE::ecs_ID ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
			{
				transform.pos += velocity.vel * dt;
			});

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
					float aspect_ratio = static_cast<float>(window->GetWidth()) / window->GetHeight();
					
					float fov = glm::radians(45.0f);
					float focal_length = 1 / std::tan(fov * 0.5f);
					float cam_far = 100.0f;
					float cam_near = 0.1f;
					
					//camera.data.projection = glm::mat4(1.0f);
					camera.data.projection[0][0] = focal_length / aspect_ratio;
					camera.data.projection[1][1] = -focal_length;
					camera.data.projection[2][2] = cam_near / (cam_far - cam_near);
					camera.data.projection[2][3] = -1.0f;
					camera.data.projection[3][2] = (cam_far * cam_near) / (cam_far - cam_near);
					camera.data.projection[3][3] = 0.0f;

					//context->global_data = camera.data;
					//TODO: WHY THIS PROJECTION MATRIX STILL WORKS????
					//camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
					//camera.data.projection[1][1] *= -1;
					camera.data.view = camera.GetCameraMat();
					camera.data.inv_view = glm::inverse(glm::transpose(camera.data.view));

					//TODO: pass renderer camera data
					context->global_data = camera.data;
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
					light.data.pos = transform.pos;
					//material.data.diffuse = light.data.diffuse;
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
	}

	void Engine::Init()
	{
		Engine& engine = Get();
		scene_serializer.Init();
		ECS_TestSetup();
		
		//Init window
		window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Ouroboros Project"));
		window->Init();
		OE::Input::Init();
		window->vulkan_imgui_manager.Init(GetGLFWWindow());

		camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 100.0f, 0.1f);
		camera.data.projection[1][1] *= -1;
		camera.data.view = camera.GetCameraMat();
		camera.data.position = glm::vec3(0.f, 0.f, 6.0);

		window->GetWindowData().RenderContextData->InitGlobalData();

		//init engine module
		delta_timer.Init();

		InitEssentialAssets();
	}

	void Engine::PreUpdate()
	{
		glfwPollEvents();
		delta_timer.PreUpdate();
		for (auto& event_function : event_functions[EventFunctionType::PRE])
		{
			event_function();
		}
		event_functions[EventFunctionType::PRE].clear();
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

			if (Input::GetKeyboardButton(GLFW_KEY_W).IsDown())
			{
				Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::FORWARD, move_velocity);
			}
			if (Input::GetKeyboardButton(GLFW_KEY_S).IsDown())
			{
				Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::BACKWARD, move_velocity);
			}
			if (Input::GetKeyboardButton(GLFW_KEY_A).IsDown())
			{
				Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::LEFT, move_velocity);
			}
			if (Input::GetKeyboardButton(GLFW_KEY_D).IsDown())
			{
				Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::RIGHT, move_velocity);
			}
			if (Input::GetKeyboardButton(GLFW_KEY_Q).IsDown())
			{
				Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::UP, move_velocity);
			}
			if (Input::GetKeyboardButton(GLFW_KEY_R).IsDown())
			{
				Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::DOWN, move_velocity);
			}

			const glm::ivec2 mouse_move = Input::GetMouse().GetCursorMove();
			Engine::camera.MouseInput(-mouse_move.x, -mouse_move.y, mouse_move_velocity);
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

		for (auto& event_function : event_functions[EventFunctionType::ONUPDATE])
		{
			event_function();
		}
		event_functions[EventFunctionType::ONUPDATE].clear();
	}

	void Engine::PostUpdate()
	{
		for (auto& event_function : event_functions[EventFunctionType::POST])
		{
			event_function();
		}
		event_functions[EventFunctionType::POST].clear();

		window->BeginFrame();
		window->Update();
		gui_manager.Update();
		for (const auto& event_function : event_functions[EventFunctionType::START_OF_RENDERER_CONTEXT])
		{
			dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->AddStartContextEvent(event_function);
		}
		event_functions[EventFunctionType::START_OF_RENDERER_CONTEXT].clear();
		for (const auto& event_function : event_functions[EventFunctionType::END_OF_RENDERER_END_FRAME])
		{
			dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->AddAfterEndDeferredEvent(event_function);
		}
		event_functions[EventFunctionType::END_OF_RENDERER_END_FRAME].clear();
		for (const auto& event_function : event_functions[EventFunctionType::END_OF_RENDERER_CONTEXT])
		{
			dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->AddEndContextEvent(event_function);
		}
		event_functions[EventFunctionType::END_OF_RENDERER_CONTEXT].clear();

		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->DrawQueue();

		window->EndFrame();
		Input::Update();
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
