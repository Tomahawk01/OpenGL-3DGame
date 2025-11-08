#include "SystemInfo.h"

#include "Renderer/OpenGL.h"
#include "Exception.h"
#include "Logger.h"
#include "WMI.h"

#include <ranges>
#include <format>

namespace {

	std::string GetOSVersion(const Game::WMI& wmi)
	{
		try
		{
			const auto caption = wmi.Query("SELECT * FROM Win32_OperatingSystem", "Caption") | std::views::join_with(' ') | std::ranges::to<std::string>();
			const auto version = wmi.Query("SELECT * FROM Win32_OperatingSystem", "Version") | std::views::join_with(' ') | std::ranges::to<std::string>();

			return std::format("{} ({})", caption, version);
		}
		catch (Game::Exception& e)
		{
			Game::Logger::Error("Failed to get OS version: {}", e);
		}
		catch (...)
		{
			Game::Logger::Error("Failed to get OS version");
		}

		return {};
	}

	std::string GetGPUID(const Game::WMI& wmi)
	{
		try
		{
			const auto props = wmi.Query("SELECT * FROM Win32_VideoController", "Caption");
			return props | std::views::join_with(' ') | std::ranges::to<std::string>();
		}
		catch (Game::Exception& e)
		{
			Game::Logger::Error("Failed to get GPU id: {}", e);
		}
		catch (...)
		{
			Game::Logger::Error("Failed to get GPU id");
		}

		return {};
	}

	std::string GetGPUDriver(const Game::WMI& wmi)
	{
		try
		{
			const auto props = wmi.Query("SELECT * FROM Win32_VideoController", "DriverVersion");
			return props | std::views::join_with(' ') | std::ranges::to<std::string>();
		}
		catch (Game::Exception& e)
		{
			Game::Logger::Error("Failed to get GPU driver version: {}", e);
		}
		catch (...)
		{
			Game::Logger::Error("Failed to get GPU driver version");
		}

		return {};
	}

	std::string GetSystemMemory()
	{
		::ULONGLONG mem{};
		::GetPhysicallyInstalledSystemMemory(&mem);

		return mem == 0 ? "" : std::format("{} MB", mem / 1024ull);
	}

}

namespace Game {

	SystemInfo GetSystemInfo()
	{
		WMI wmi{};
		return {
			.osVersion = GetOSVersion(wmi),
			.gpuID = GetGPUID(wmi),
			.gpuDriver = GetGPUDriver(wmi),
			.systemMemory = GetSystemMemory()
		};
	}

	std::string to_string(const SystemInfo& info)
	{
		const auto getOrEmpty =
			[](const auto& s)
			{
				return s.empty() ? "unknown" : s;
			};

		return std::format("\nOS version: {}\nGPU id: {}\nGPU driver: {}\nSystem memory: {}",
						   getOrEmpty(info.osVersion),
						   getOrEmpty(info.gpuID),
						   getOrEmpty(info.gpuDriver),
						   getOrEmpty(info.systemMemory));
	}

}
