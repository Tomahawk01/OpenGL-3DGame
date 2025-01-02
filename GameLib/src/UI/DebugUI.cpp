#include "DebugUI.h"

#include "Math/Matrix4.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>
#include <ImGuizmo.h>

#include <ranges>

namespace Game {

	DebugUI::DebugUI(HWND window, Scene& scene, Camera& camera, float& gamma)
		: m_Scene(scene)
		, m_Camera(camera)
		, m_Gamma(gamma)
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

		static std::size_t selectedPoint{};

		if (::ImGui::Button("Add Light"))
		{
			m_Scene.pointLights.push_back(m_Scene.pointLights.back());
			selectedPoint = m_Scene.pointLights.size() - 1u;
		}

		::ImGui::LabelText("FPS", "%0.1f", io.Framerate);
		::ImGui::SliderFloat("Gamma", &m_Gamma, 0.0f, 5.0f);

		if (::ImGui::CollapsingHeader("Ambient"))
		{
			float color[3]{};
			std::memcpy(color, &m_Scene.ambient, sizeof(color));

			if (::ImGui::ColorPicker3("Ambient color", color))
			{
				std::memcpy(&m_Scene.ambient, color, sizeof(color));
			}
		}

		if (::ImGui::CollapsingHeader("Directional"))
		{
			float color[3]{};
			std::memcpy(color, &m_Scene.directionalLight.color, sizeof(color));

			if (::ImGui::ColorPicker3("Directional color", color))
			{
				std::memcpy(&m_Scene.directionalLight.color, color, sizeof(color));
			}
		}

		for (const auto& [index, point] : m_Scene.pointLights | std::views::enumerate)
		{
			float color[3]{};
			std::memcpy(color, &point.color, sizeof(color));

			const std::string headerName = std::format("Color {}", index);
			const std::string pickerName = std::format("Color {}", index);
			const std::string constName = std::format("Constant {}", index);
			const std::string linearName = std::format("Linear {}", index);
			const std::string quadName = std::format("Quadratic {}", index);

			if (::ImGui::CollapsingHeader(headerName.c_str()))
			{
				if (::ImGui::ColorPicker3(pickerName.c_str(), color))
				{
					point.color.r = color[0];
					point.color.g = color[1];
					point.color.b = color[2];

					selectedPoint = index;
				}

				::ImGui::SliderFloat(constName.c_str(), &point.constAttenuation, 0.0f, 1.0f);
				::ImGui::SliderFloat(linearName.c_str(), &point.linearAttenuation, 0.0f, 1.0f);
				::ImGui::SliderFloat(quadName.c_str(), &point.quadAttenuation, 0.0f, 0.1f);
			}
		}

		auto& point = m_Scene.pointLights[selectedPoint];
		mat4 translate{ point.position };

		::ImGuizmo::Manipulate(
			m_Camera.GetView().data(),
			m_Camera.GetProjection().data(),
			::ImGuizmo::TRANSLATE,
			::ImGuizmo::WORLD,
			const_cast<float*>(translate.data().data()),
			nullptr, nullptr, nullptr, nullptr);

		point.position.x = translate.data()[12];
		point.position.y = translate.data()[13];
		point.position.z = translate.data()[14];

		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
	}

	void DebugUI::AddMouseEvent(const MouseButtonEvent& e) const
	{
		::ImGuiIO& io = ::ImGui::GetIO();
		io.AddMouseButtonEvent(0, e.GetState() == MouseButtonState::DOWN);
	}

}
