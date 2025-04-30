#include <Utilities/Error.h>
#include <Utilities/Logger.h>
#include <Game/Game.h>

#include <iostream>
#include <print>

int main(int argc, char** argv)
{
	Game::Logger::Info("Starting Game...");

	try
	{
		Game::Ensure(argc == 2, "game.exe <root_path>");

		Game::Game app{};
		app.Run(argv[1]);
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
