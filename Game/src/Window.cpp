#include "Window.h"

#include "Utilities/Error.h"
#include "OpenGL.h"

#include <print>

namespace {

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{};
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB{};

	static bool g_Running = true;

	void APIENTRY OpenGLDebugCallback(
		::GLenum src,
		::GLenum type,
		::GLuint id,
		::GLenum severity,
		::GLsizei,
		const ::GLchar* message,
		const void*)
	{
		std::println("{} {} {} {} {}", src, type, id, severity, message);
	}

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

		ResolveWGLFunctions(m_WndClass.hInstance);
		InitOpenGL(m_DeviceCtx);
		ResolveGlobalGLFunctions();
		SetupDebug();

		::glEnable(GL_DEPTH_TEST);
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

	void Window::Swap() const
	{
		::SwapBuffers(m_DeviceCtx);
	}

}
