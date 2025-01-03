#include "Window.h"

#include "OpenGL.h"
#include "Logger.h"
#include "Utilities/Error.h"
#include "Events/Event.h"

#include <windowsx.h>
#include <hidusage.h>

#include <print>
#include <queue>
#include <ranges>

namespace {

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{};
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB{};

	static std::queue<Game::Event> g_EventQueue{};

	void APIENTRY OpenGLDebugCallback(
		::GLenum src,
		::GLenum type,
		::GLuint id,
		::GLenum severity,
		::GLsizei,
		const ::GLchar* message,
		const void*)
	{
		if (type == GL_DEBUG_TYPE_ERROR)
			throw Game::Exception(std::format("{} {} {} {} {}", src, type, id, severity, message));

		//Game::Logger::Info("{} {} {} {} {}", src, type, id, severity, message);
	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg)
		{
		case WM_CLOSE:
		{
			g_EventQueue.emplace(Game::StopEvent{});
		} break;
		case WM_KEYUP:
		{
			g_EventQueue.emplace(Game::KeyEvent{ static_cast<Game::Key>(wParam), Game::KeyState::UP });
		} break;
		case WM_KEYDOWN:
		{
			g_EventQueue.emplace(Game::KeyEvent{ static_cast<Game::Key>(wParam), Game::KeyState::DOWN });
		} break;
		case WM_INPUT:
		{
			::RAWINPUT raw{};
			::UINT dwSize{ sizeof(::RAWINPUT) };
			Game::Ensure(::GetRawInputData(reinterpret_cast<::HRAWINPUT>(lParam), RID_INPUT, &raw, &dwSize, sizeof(::RAWINPUTHEADER)) != static_cast<::UINT>(-1), "Failed to get raw input");

			if (raw.header.dwType == RIM_TYPEMOUSE)
			{
				const auto x = raw.data.mouse.lLastX;
				const auto y = raw.data.mouse.lLastY;

				g_EventQueue.emplace(Game::MouseEvent{ static_cast<float>(x), static_cast<float>(y) });
			}
		} break;
		case WM_LBUTTONUP:
		{
			g_EventQueue.emplace(Game::MouseButtonEvent{ static_cast<float>(GET_X_LPARAM(lParam)),
														 static_cast<float>(GET_Y_LPARAM(lParam)),
														 Game::MouseButtonState::UP });
		} break;
		case WM_LBUTTONDOWN:
		{
			g_EventQueue.emplace(Game::MouseButtonEvent{ static_cast<float>(GET_X_LPARAM(lParam)),
														 static_cast<float>(GET_Y_LPARAM(lParam)),
														 Game::MouseButtonState::DOWN });
		} break;
		}

		return ::DefWindowProcA(hWnd, Msg, wParam, lParam);
	}

	template<class T>
	void ResolveGLFunction(T& function, const std::string& name)
	{
		const PROC address = ::wglGetProcAddress(name.c_str());
		Game::Ensure(address != nullptr, "Could not resolve {}", name);

		function = reinterpret_cast<T>(address);
	}

	void ResolveWGLFunctions(HINSTANCE instance)
	{
		WNDCLASSA wndClass = ::WNDCLASSA{
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = ::DefWindowProcA,
			.hInstance = instance,
			.lpszClassName = "dummy window"
		};
		Game::Ensure(::RegisterClassA(&wndClass) != 0, "Could not register dummy window");

		auto dummyWindow = Game::AutoRelease<::HWND>{
			::CreateWindowExA(
				0,
				wndClass.lpszClassName,
				wndClass.lpszClassName,
				0,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0,
				0,
				wndClass.hInstance,
				0),
			::DestroyWindow
		};
		Game::Ensure(dummyWindow, "Could not create dummy window");

		auto dc = Game::AutoRelease<::HDC>{
			::GetDC(dummyWindow),
			[&dummyWindow](auto dc) { ::ReleaseDC(dummyWindow, dc); }
		};
		Game::Ensure(dc, "Could not get dummy draw context");

		auto pfd = ::PIXELFORMATDESCRIPTOR{
			.nSize = sizeof(::PIXELFORMATDESCRIPTOR),
			.nVersion = 1,
			.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			.iPixelType = PFD_TYPE_RGBA,
			.cColorBits = 32,
			.cAlphaBits = 8,
			.cDepthBits = 24,
			.cStencilBits = 8,
			.iLayerType = PFD_MAIN_PLANE
		};

		int pixelFormat = ::ChoosePixelFormat(dc, &pfd);
		Game::Ensure(pixelFormat != 0, "Failed to choose pixel format");

		Game::Ensure(::SetPixelFormat(dc, pixelFormat, &pfd) == TRUE, "Failed to set pixel format");

		const auto context = Game::AutoRelease<::HGLRC>{
			::wglCreateContext(dc),
			::wglDeleteContext
		};
		Game::Ensure(context, "Failed to create wgl context");

		Game::Ensure(::wglMakeCurrent(dc, context) == TRUE, "Could not make current context");

		// ========== Resolve GL functions here ==========

		ResolveGLFunction(wglCreateContextAttribsARB, "wglCreateContextAttribsARB");
		ResolveGLFunction(wglChoosePixelFormatARB, "wglChoosePixelFormatARB");

		Game::Ensure(::wglMakeCurrent(dc, 0) == TRUE, "Could not unbind context");
	}

	void InitOpenGL(::HDC dc)
	{
		int pixelFormatAttribs[]{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			0
		};

		int pixelFormat = 0;
		UINT numFormats = UINT{};

		::wglChoosePixelFormatARB(dc, pixelFormatAttribs, 0, 1, &pixelFormat, &numFormats);
		Game::Ensure(numFormats != 0u, "Failed to choose a pixel format");

		PIXELFORMATDESCRIPTOR pfd = ::PIXELFORMATDESCRIPTOR{};
		Game::Ensure(::DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd) != 0, "Failed to describe pixel format");
		Game::Ensure(::SetPixelFormat(dc, pixelFormat, &pfd) == TRUE, "Failed to set pixel format");

		int glAttribs[]{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		HGLRC context = ::wglCreateContextAttribsARB(dc, 0, glAttribs);
		Game::Ensure(context != nullptr, "Failed to create gl context");

		Game::Ensure(::wglMakeCurrent(dc, context) == TRUE, "Failed to make current context");
	}

	void ResolveGlobalGLFunctions()
	{
#define RESOLVE(TYPE, NAME) ResolveGLFunction(NAME, #NAME);

		FOR_OPENGL_FUNCTIONS(RESOLVE)
	}

	void SetupDebug()
	{
		::glEnable(GL_DEBUG_OUTPUT);
		::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		::glDebugMessageCallback(OpenGLDebugCallback, nullptr);
	}

}

namespace Game {

	Window::Window(std::uint32_t width, std::uint32_t height)
		: m_Window({})
		, m_DeviceCtx({})
		, m_WndClass({})
		, m_Width(width)
		, m_Height(height)
	{
		m_WndClass = {
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = WindowProc,
			.hInstance = ::GetModuleHandleA(nullptr),
			.lpszClassName = "window class"
		};
		Ensure(::RegisterClassA(&m_WndClass) != 0, "Failed to register class");

		::RECT rect{
			.left = {},
			.top = {},
			.right = static_cast<int>(width),
			.bottom = static_cast<int>(height)
		};
		Ensure(::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false) != 0, "Failed to resize window");

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

		m_DeviceCtx = Game::AutoRelease<::HDC>{
			::GetDC(m_Window),
			[this](auto dc) { ::ReleaseDC(m_Window, dc); }
		};

		::ShowWindow(m_Window, SW_SHOW);
		::UpdateWindow(m_Window);

		const ::RAWINPUTDEVICE rid{
			.usUsagePage = HID_USAGE_PAGE_GENERIC,
			.usUsage = HID_USAGE_GENERIC_MOUSE,
			.dwFlags = RIDEV_INPUTSINK,
			.hwndTarget = m_Window
		};
		Ensure(::RegisterRawInputDevices(&rid, 1, sizeof(rid)) == TRUE, "Failed to register input device");

		ResolveWGLFunctions(m_WndClass.hInstance);
		InitOpenGL(m_DeviceCtx);
		ResolveGlobalGLFunctions();
		SetupDebug();

		::glEnable(GL_DEPTH_TEST);
	}

	std::optional<Event> Window::PollEvent() const
	{
		MSG message = ::MSG{};
		while (::PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			::TranslateMessage(&message);
			::DispatchMessageA(&message);
		}

		if (!std::ranges::empty(g_EventQueue))
		{
			const Game::Event event = g_EventQueue.front();
			g_EventQueue.pop();
			return event;
		}

		return {};
	}

	void Window::Swap() const
	{
		::SwapBuffers(m_DeviceCtx);
	}

	HWND Window::GetNativeHandle() const
	{
		return m_Window;
	}

	std::uint32_t Window::GetWidth() const
	{
		return m_Width;
	}

	std::uint32_t Window::GetHeight() const
	{
		return m_Height;
	}

}
