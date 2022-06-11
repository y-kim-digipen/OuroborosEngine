#include "engine.h"
#include <chrono>
#include <gtc/matrix_transform.hpp>

#include "engine_settings.h"
#include "gui/gui_component_panel.h"

namespace OE
{
	void Engine::SetupGUI()
	{
		window->vulkan_imgui_manager.RegisterMainMenu([]() mutable
			{
				if (ImGui::BeginMainMenuBar())
				{
					if (ImGui::BeginMenu("Editor"))
					{
						auto& panels = window->vulkan_imgui_manager.GetPanels();
						for (auto& [key, pair] : panels)
						{
							bool& open = pair.second;
							if (ImGui::MenuItem(key.c_str(), nullptr, open))
							{
								open = !open;
							}
						}
						ImGui::EndMenu();
					}
					ImGui::EndMainMenuBar();
				}
			});

		window->vulkan_imgui_manager.RegisterPanel("EngineInfo", &OE::EngineInfoPanelFunction);
		window->vulkan_imgui_manager.RegisterPanel("Entities", &OE::EntityInfoPanelFunction);
		window->vulkan_imgui_manager.RegisterPanel("SystemInfo", &OE::SystemInfoPanelFunction);
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
					context->global_data.position = camera.data.position;
					context->global_data.view = camera.data.view;
					context->global_data.projection = camera.data.projection;
					context->UpdateGlobalData();
					context->BindGlobalData();

					context->AddDrawQueue(&transform, nullptr, &mesh, &shader);
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
		camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
		camera.data.view = camera.GetCameraMat();



		Renderer::ShaderConfig shader_config
		{		"shader",
		{	Renderer::E_StageType::VERTEX_SHADER,
					Renderer::E_StageType::FRAGMENT_SHADER },
			2 };

		Renderer::ShaderConfig shader_config2{
					"shader2",
			{	Renderer::E_StageType::VERTEX_SHADER,
						Renderer::E_StageType::FRAGMENT_SHADER	},2};

		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->shader_manager_.AddShader(&shader_config);
		dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->shader_manager_.AddShader(&shader_config2);
		window->GetWindowData().RenderContextData.get()->InitGlobalData();

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

		SetupModule();
	}

	void Engine::PreUpdate()
	{
		delta_timer.PreUpdate();
	}

	void Engine::Update()
	{
		input.Update();
		ecs_manager.UpdateSystem(OE::Engine::Get().delta_timer.GetDeltaTime());
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
