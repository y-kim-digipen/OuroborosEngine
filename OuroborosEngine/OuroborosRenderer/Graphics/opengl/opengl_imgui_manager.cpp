#include "opengl_imgui_manager.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
namespace Renderer
{
	OpenglImguiManager::OpenglImguiManager() : ImguiManager()
	{
	}

	void OpenglImguiManager::Init(GLFWwindow* window)
	{
		ImGui::CreateContext();
		ImGuiIO m_io = ImGui::GetIO();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
	
	}

	void OpenglImguiManager::BeginFrame()
	{
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

	}

	void OpenglImguiManager::EndFrame()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void OpenglImguiManager::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

	}

	void OpenglImguiManager::Update()
	{

	}
}
