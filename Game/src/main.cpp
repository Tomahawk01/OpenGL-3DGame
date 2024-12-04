#include "Window.h"
#include "Exception.h"

#include <iostream>
#include <print>

int main()
{
	std::println("hello world");

	try
	{
		Game::Window window{ 800u, 600u };

		while (window.IsRunning())
		{

		}
	}
	catch (const Game::Exception& err)
	{
		std::println(std::cerr, "Exception {}", err);
	}
	catch (...)
	{
		std::println(std::cerr, "Unknown exception");
	}

	return 0;
}
