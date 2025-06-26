#include <Utilities/Error.h>
#include <Utilities/Logger.h>
#include <Game/Game.h>
#include <Game/config.h>

#include <iostream>
#include <print>

int main(int argc, char** argv)
{
	Game::Logger::Info("Starting Game v{}.{}.{}", Game::Version::MAJOR, Game::Version::MINOR, Game::Version::PATCH);

	try
	{
		const std::string root = (argc == 2) ? std::string{ argv[1] } : ".";

		Game::Game app{};
		app.Run(root);
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
