#include "engine.h"
#include <chrono>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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
					camera.data.projection[1][1] *= -1;
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

	}

	void Engine::SetupModule()
	{
		asset_manager.GetManager<MeshAssetManager>().LoadAsset("suzanne.obj");
		asset_manager.GetManager<ShaderAssetManager>().LoadAsset("shader");
		asset_manager.GetManager<ShaderAssetManager>().LoadAsset("shader2");
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
		/*
		target_fps = 240;
		target_dt = 1.0f / (target_fps * 2);*/
		//Init engine
		Engine& engine = Get();

		ECS_TestSetup();
		
		//Init window
		window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Ouroboros Project"));

		glfwSetKeyCallback(GetGLFWWindow(), GLFW_Keyboard_Callback);
		glfwSetMouseButtonCallback(GetGLFWWindow(), GLFW_MouseButton_Callback);


		camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
		camera.data.projection[1][1] *= -1;
		camera.data.view = camera.GetCameraMat();

		Renderer::ShaderConfig shader_config3{
					"light_shader",
			{	Renderer::E_StageType::VERTEX_SHADER,
						Renderer::E_StageType::FRAGMENT_SHADER	},2 };

		(window->GetWindowData().RenderContextData.get())->shader_manager->AddShader(&shader_config3);

		window->GetWindowData().RenderContextData->InitGlobalData();

		//init engine module
		SetupGUI();
		delta_timer.Init();

		std::vector<int> key_supports{
			GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C,
			GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_D,
			GLFW_KEY_SPACE,
			GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT,
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

		
		(window->GetWindowData().RenderContextData.get())->material_manager->AddMaterial("material", Asset::MaterialData());
		SetupModule();


	}

	void Engine::PreUpdate()
	{
		glfwPollEvents();
		delta_timer.PreUpdate();
	}
	 
	void Engine::Update()
	{
		input.Update();
		if(Input::Down( GLFW_KEY_W) )
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::FORWARD, DeltaTime::GetDeltaTime());
		}
		if (Input::Down(GLFW_KEY_S) )
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::BACKWARD, DeltaTime::GetDeltaTime());
		}
		if (Input::Down(GLFW_KEY_A) )
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::LEFT, DeltaTime::GetDeltaTime());
		}
		if (Input::Down(GLFW_KEY_D) )
		{
			Engine::camera.KeyboardInput(Renderer::Camera_MoveTo::RIGHT, DeltaTime::GetDeltaTime());
		}

		if(Input::Down(GLFW_MOUSE_BUTTON_RIGHT))
		{
		/*	const float offset = DeltaTime::GetDeltaTime() * 5.f;
			if(Input::glfw)
			Engine::camera.MouseInput(0,)*/

		}
		ecs_manager.UpdateSystem(OE::Engine::Get().delta_timer.GetDeltaTime());

	/*	if(input.Down(GLFW_MOUSE_BUTTON_LEFT))
		{
			std::cout << "Down" << std::endl;
		}
		else {
			std::cout << "Up" << std::endl;
		}*/
	}

	void Engine::PostUpdate()
	{
		window->BeginFrame();
		window->Update();
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
