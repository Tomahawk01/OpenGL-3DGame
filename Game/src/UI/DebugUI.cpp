#include "DebugUI.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>

namespace Game {

	DebugUI::DebugUI(HWND window, Scene& scene)
		: m_Scene(scene)
	{
		IMGUI_CHECKVERSION();
		::ImGui::CreateContext();
		::ImGuiIO& io = ::ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		::ImGui::StyleColorsDark();

		::ImGui_ImplWin32_InitForOpenGL(window);
		::ImGui_ImplOpenGL3_Init();
	}

	DebugUI::~DebugUI()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplWin32_Shutdown();
		::ImGui::DestroyContext();
	}

	void DebugUI::Render() const
	{
		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();

		static float color[3]{};
		if (::ImGui::ColorPicker3("", color))
		{
			m_Scene.pointLight.color.r = color[0];
			m_Scene.pointLight.color.g = color[1];
			m_Scene.pointLight.color.b = color[2];
		}

		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
	}

	void DebugUI::AddMouseEvent(const MouseButtonEvent& e) const
	{
		::ImGuiIO& io = ::ImGui::GetIO();
		io.AddMouseButtonEvent(0, e.GetState() == MouseButtonState::DOWN);
	}

}
