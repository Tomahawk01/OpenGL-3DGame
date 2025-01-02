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
		Window(std::uint32_t width, std::uint32_t height);
		~Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = default;
		Window& operator=(Window&&) = default;

		std::optional<Event> PollEvent() const;
		void Swap() const;

		HWND GetNativeHandle() const;
		std::uint32_t GetWidth() const;
		std::uint32_t GetHeight() const;

	private:
		AutoRelease<::HWND, nullptr> m_Window;
		AutoRelease<::HDC> m_DeviceCtx;
		::WNDCLASSA m_WndClass;
		std::uint32_t m_Width;
		std::uint32_t m_Height;
	};

}
