#pragma once

#include "Utilities/AutoRelease.h"
#include "Events/Event.h"

#include <cstdint>
#include <optional>

#include <Windows.h>

namespace Game {

	class Window
	{
	public:
		Window(uint32_t width, uint32_t height, uint32_t x, uint32_t y);
		~Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = default;
		Window& operator=(Window&&) = default;

		std::optional<Event> PollEvent() const;
		void Swap() const;

		HWND GetNativeHandle() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		AutoRelease<::HWND, nullptr> m_Window;
		AutoRelease<::HDC> m_DeviceCtx;
		::WNDCLASSA m_WndClass;
		uint32_t m_Width;
		uint32_t m_Height;
	};

}
