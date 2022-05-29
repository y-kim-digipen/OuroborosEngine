#ifndef OPENGL_IMGUI_MANAGER_H
#define OPENGL_IMGUI_MANAGER_H

#include "../imgui_manager.h"

namespace Renderer
{
	
class OpenglImguiManager : public ImguiManager
{
public:
	OpenglImguiManager();
	void Init(GLFWwindow* window) override;
	void BeginFrame() override;
	void EndFrame() override;
	void Shutdown() override;
	void Update() override;

};
}




#endif //!OPENGL_IMGUI_MANAGER_H