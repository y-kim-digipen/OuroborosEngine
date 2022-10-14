#include "engine.h"
#include <chrono>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <gtc/matrix_transform.hpp>

#include "engine_settings.h"
#include "Graphics/vulkan/vulkan_shader.h"
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
					camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
					camera.data.projection[1][1] *= -1;
					camera.data.view = camera.GetCameraMat();
				
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

	void Engine::GLFW_Keyboard_Callback(GLFWwindow* p_window, int key, int scancode, int action, int mods)
	{
		input.KeyboardCallback(p_window, key, scancode, action, mods);
		window->vulkan_imgui_manager.GLFW_KeyboardCallback(p_window, key, scancode, action, mods);
	}

	void Engine::GLFW_MouseButton_Callback(GLFWwindow* p_window, int key, int action, int mods)
	{
		input.MouseButtonCallback(p_window, key, action, mods);
		window->vulkan_imgui_manager.GLFW_MouseButtonCallback(p_window, key, action, mods);
	}

	void Engine::Init()
	{
		Engine& engine = Get();
		scene_serializer.Init();
		ECS_TestSetup();
		
		//Init window
		window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Ouroboros Project"));

		glfwSetKeyCallback(GetGLFWWindow(), GLFW_Keyboard_Callback);
		glfwSetMouseButtonCallback(GetGLFWWindow(), GLFW_MouseButton_Callback);


		camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
		camera.data.projection[1][1] *= -1;
		camera.data.view = camera.GetCameraMat();
		camera.data.position = glm::vec3(0.f, 0.f, 6.0);

		window->GetWindowData().RenderContextData->InitGlobalData();

		//init engine module
		delta_timer.Init();

		std::vector<int> key_supports{
			GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C,
			GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_D,
			GLFW_KEY_SPACE,
			GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT,
			GLFW_KEY_Y, GLFW_KEY_H, GLFW_KEY_G, GLFW_KEY_J
		};

		input.Init(window->GetWindowData().window, key_supports);
		input.RegisterCallback(GLFW_KEY_SPACE, [](Input::Modes mode)
			{
				//if (mode == Input::PRESSED)
				//{
				//	std::cout << "Pressed" << std::endl;
				//}
				//else
				//{
				//	std::cout << "Released" << std::endl;
				//}
			});
		
		InitEssentialAssets();

		camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
		camera.data.projection[1][1] *= -1;
		camera.data.view = camera.GetCameraMat();
		camera.data.position = glm::vec3(0.f, 0.f, 6.0);

		window->GetWindowData().RenderContextData->InitGlobalData();
		//Profiler::Start();
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
		input.Update();
		if (Input::Down(GLFW_KEY_W))
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::FORWARD, DeltaTime::GetDeltaTime());
		}
		if (Input::Down(GLFW_KEY_S))
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::BACKWARD, DeltaTime::GetDeltaTime());
		}
		if (Input::Down(GLFW_KEY_A))
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::LEFT, DeltaTime::GetDeltaTime());
		}
		if (Input::Down(GLFW_KEY_D))
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::RIGHT, DeltaTime::GetDeltaTime());
		}
		const float offset = 5.f * DeltaTime::GetDeltaTime();
		if (Input::Down(GLFW_KEY_Y))
		{
			Engine::camera.MouseInput(0, offset);
		}
		if (Input::Down(GLFW_KEY_H))
		{
			Engine::camera.MouseInput(0, -offset);
		}

		if (Input::Down(GLFW_KEY_G))
		{
			Engine::camera.MouseInput(-offset, 0);
		}
		if (Input::Down(GLFW_KEY_J))
		{
			Engine::camera.MouseInput(offset, 0);
		}
		if (Input::Down(GLFW_MOUSE_BUTTON_RIGHT))
		{

		}

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
		input.Update();

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

	double Engine::DeltaTime::GetDeltaTime()
	{
		return dt;
	}

}
