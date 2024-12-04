#pragma once

#include "AutoRelease.h"

#include <cstdint>
#include <Windows.h>

namespace Game {

	class Window
	{
	public:
		Window(std::uint32_t width, std::uint32_t height);
		~Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = default;
		Window& operator=(Window&&) = default;

		bool IsRunning() const;

	private:
		AutoRelease<::HWND, nullptr> m_Window;
		AutoRelease<::HDC> m_DeviceCtx;
		::WNDCLASSA m_WndClass;
	};

}
