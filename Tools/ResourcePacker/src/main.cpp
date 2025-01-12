#include <print>
#include <iostream>

#include "Utilities/Exception.h"

int main()
{
	try
	{
		std::println("Resource packer");
	}
	catch (Game::Exception& e)
	{
		std::println(std::cerr, "{}", e);
	}

	return 0;
}
