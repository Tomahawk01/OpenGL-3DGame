#pragma once

#include "Utilities/AutoRelease.h"
#include "Events/Event.h"

#include <cstdint>
#include <optional>
#include <string>

#include <Windows.h>

namespace Game {

	enum class WindowMode
	{
		FULLSCREEN,
		WINDOWED
	};

	class Window
	{
	public:
		Window(WindowMode mode, uint32_t width, uint32_t height, uint32_t x, uint32_t y);
		~Window() = default;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&&) = default;
		Window& operator=(Window&&) = default;

		std::optional<Event> PollEvent() const;
		void Swap() const;

		void SetTitle(const std::string& title) const;
		void SetMode(WindowMode mode);

		HWND GetNativeHandle() const;
		uint32_t GetRenderWidth() const;
		uint32_t GetRenderHeight() const;
		uint32_t GetWindowWidth() const;
		uint32_t GetWindowHeight() const;
		WindowMode GetMode() const;

	private:
		AutoRelease<::HWND, nullptr> m_Window;
		AutoRelease<::HDC> m_DeviceCtx;
		::WNDCLASSA m_WndClass;
		uint32_t m_Width;
		uint32_t m_Height;
		WindowMode m_Mode;
	};

	inline std::string to_string(WindowMode mode)
	{
		switch (mode)
		{
			case Game::WindowMode::FULLSCREEN:
				return "FULLSCREEN";
			case Game::WindowMode::WINDOWED:
				return "WINDOWED";
			default:
				return "UNKNOWN";
		}
	}

}
