#pragma once

#include "Events/MouseButtonEvent.h"
#include "Core/Scene.h"
#include "Renderer/Camera.h"

#include <Windows.h>

namespace Game {

	class DebugUI
	{
	public:
		DebugUI(HWND window, Scene& scene, const Camera& camera, float& gamma);
		~DebugUI();

		void Render() const;
		void AddMouseEvent(const MouseButtonEvent& e) const;

	private:
		Scene& m_Scene;
		const Camera& m_Camera;
		float& m_Gamma;
	};

}
