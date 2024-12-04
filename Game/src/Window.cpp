#include "Window.h"

#include <stdexcept>
#include <print>

namespace {

	bool g_Running = true;

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg)
		{
		case WM_CLOSE:
			std::println("Closing window");
			g_Running = false;
			break;
		case WM_KEYDOWN:
			std::println("Key down");
			break;
		}

		return ::DefWindowProcA(hWnd, Msg, wParam, lParam);
	}

}

namespace Game {

	Window::Window(std::uint32_t width, std::uint32_t height)
		: m_Window({}), m_WndClass({})
	{
		m_WndClass = {
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = WindowProc,
			.hInstance = ::GetModuleHandleA(nullptr),
			.lpszClassName = "window class"
		};

		if (::RegisterClassA(&m_WndClass) == 0)
			throw std::runtime_error("Failed to register class");

		::RECT rect{
			.left = {},
			.top = {},
			.right = static_cast<int>(width),
			.bottom = static_cast<int>(height)
		};

		if (::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) == 0)
			throw std::runtime_error("Could not resize window");

		m_Window = { ::CreateWindowExA(
			0,
			m_WndClass.lpszClassName,
			"game window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			m_WndClass.hInstance,
			nullptr),
		::DestroyWindow };

		::ShowWindow(m_Window, SW_SHOW);
		::UpdateWindow(m_Window);
	}

	bool Window::IsRunning() const
	{
		MSG message = ::MSG{};
		while (::PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			::TranslateMessage(&message);
			::DispatchMessageA(&message);
		}

		return g_Running;
	}

}
