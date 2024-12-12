#pragma once

#include "Events/MouseButtonEvent.h"
#include "Scene.h"

#include <Windows.h>

#include <imgui.h>

namespace Game {

	class DebugUI
	{
	public:
		DebugUI(HWND window, Scene& scene);
		~DebugUI();

		void Render() const;
		void AddMouseEvent(const MouseButtonEvent& e) const;

	private:
		Scene& m_Scene;
	};

}
