#include <Utilities/Error.h>
#include <Utilities/Logger.h>
#include <Game/Game.h>
#include <Game/config.h>
#include <Utilities/SystemInfo.h>

#include <iostream>
#include <print>

int main(int argc, char** argv)
{
	Game::Logger::Info("Starting Game v{}.{}.{}", Game::Version::MAJOR, Game::Version::MINOR, Game::Version::PATCH);

	const auto args = std::vector<std::string_view>(argv + 1u, argv + argc);

	try
	{
		Game::Ensure(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED) == S_OK, "Failed to initialize com");
		Game::Logger::Info("{}", Game::GetSystemInfo());

		const auto root = !args.empty() ? args.front() : ".";
		Game::Game app{ args };
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
