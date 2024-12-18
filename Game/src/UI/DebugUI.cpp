#include "DebugUI.h"

#include "Math/Matrix4.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>
#include <ImGuizmo.h>

namespace Game {

	DebugUI::DebugUI(HWND window, Scene& scene, Camera& camera)
		: m_Scene(scene)
		, m_Camera(camera)
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
		::ImGuiIO& io = ::ImGui::GetIO();

		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();

		::ImGuizmo::SetOrthographic(false);
		::ImGuizmo::BeginFrame();
		::ImGuizmo::Enable(true);
		::ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		mat4 translate{ m_Scene.pointLight.position };

		::ImGuizmo::Manipulate(
			m_Camera.GetView().data(),
			m_Camera.GetProjection().data(),
			::ImGuizmo::TRANSLATE,
			::ImGuizmo::WORLD,
			const_cast<float*>(translate.data().data()),
			nullptr, nullptr, nullptr, nullptr);

		m_Scene.pointLight.position.x = translate.data()[12];
		m_Scene.pointLight.position.y = translate.data()[13];
		m_Scene.pointLight.position.z = translate.data()[14];

		::ImGui::LabelText("FPS", "%0.1f", io.Framerate);

		static float color[3]{};
		if (::ImGui::ColorPicker3("", color))
		{
			m_Scene.pointLight.color.r = color[0];
			m_Scene.pointLight.color.g = color[1];
			m_Scene.pointLight.color.b = color[2];
		}

		::ImGui::SliderFloat("Constant", &m_Scene.pointLight.constAttenuation, 0.0f, 1.0f);
		::ImGui::SliderFloat("Linear", &m_Scene.pointLight.linearAttenuation, 0.0f, 1.0f);
		::ImGui::SliderFloat("Quadratic", &m_Scene.pointLight.quadAttenuation, 0.0f, 1.0f);

		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
	}

	void DebugUI::AddMouseEvent(const MouseButtonEvent& e) const
	{
		::ImGuiIO& io = ::ImGui::GetIO();
		io.AddMouseButtonEvent(0, e.GetState() == MouseButtonState::DOWN);
	}

}
