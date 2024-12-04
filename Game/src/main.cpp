#include "Window.h"
#include "Exception.h"

#include <iostream>
#include <print>

void bar()
{
	throw Game::Exception("err");
}

void foo()
{
	bar();
}

int main()
{
	std::println("hello world");

	try
	{
		foo();
	}
	catch (Game::Exception& err)
	{
		std::println(std::cerr, "exception {} {}", err.what(), err.Stacktrace());
	}

	Game::Window window{ 800u, 600u };

	while (window.IsRunning())
	{

	}

	return 0;
}
