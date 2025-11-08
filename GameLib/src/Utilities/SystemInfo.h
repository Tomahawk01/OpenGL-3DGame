#pragma once

#include <string>

namespace Game {

	struct SystemInfo
	{
		std::string osVersion;
		std::string gpuID;
		std::string gpuDriver;
		std::string systemMemory;
	};

	SystemInfo GetSystemInfo();

	std::string to_string(const SystemInfo& info);

}
