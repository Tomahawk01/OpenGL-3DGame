#include "Exception.h"
#include "OpenGL.h"
#include "Window.h"

#include <iostream>
#include <print>

int main()
{
	std::println("hello world");

	try
	{
		Game::Window window{ 800u, 600u };

		::glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// TODO: Create shaders and program
		// TODO: Link shaders and program
		// TODO: Create VBO and VAO
		// TODO: Setup all that

		while (window.IsRunning())
		{
			::glClear(GL_COLOR_BUFFER_BIT);

			// TODO: Draw

			window.Swap();
		}
	}
	catch (const Game::Exception& err)
	{
		std::println(std::cerr, "{}", err);
	}
	catch (...)
	{
		std::println(std::cerr, "Unknown exception");
	}

	return 0;
}
