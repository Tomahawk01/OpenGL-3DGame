#include "Utilities/Error.h"
#include "Utilities/AutoRelease.h"
#include "Logger.h"
#include "TLVWriter.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <print>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <set>

namespace {

	Game::TextureFormat ToTextureFormat(int numChannels)
	{
		switch (numChannels)
		{
		case 3: return Game::TextureFormat::RGB;
		case 4: return Game::TextureFormat::RGBA;
		default:
			throw Game::Exception(std::format("Unsupported number of channels: {}", numChannels));
		}
	}

	Game::TextureUsage ToTextureUsage(std::string_view path)
	{
		if (path.contains(".data"))
			return Game::TextureUsage::DATA;
		else if (path.contains(".srgb"))
			return Game::TextureUsage::SRGB;
		
		throw Game::Exception(std::format("Unsupported usage type: {}", path));
	}

}

int main(int argc, char** argv)
{
	try
	{
		std::println("Resource packer");

		Game::Ensure(argc == 3, "usage: ./ResourcePacker.exe <asset_dir> <out_path>");

		const std::set<std::string> imageExtensions{ ".png", ".jpg" };

		Game::TLVWriter writer{};

		for (const auto& entry : std::filesystem::directory_iterator{ argv[1] })
		{
			const std::string path = entry.path().string();
			const std::string ext = entry.path().extension().string();
			const std::string filename = entry.path().filename().string();
			const std::string assetName = filename.substr(0, filename.find("."));

			if (imageExtensions.contains(ext))
			{
				int w{};
				int h{};
				int numChannels{};

				Game::Ensure(::stbi_info(path.c_str(), &w, &h, &numChannels) == 1, "Failed to get image info");
				auto rawData = std::unique_ptr<::stbi_uc, void(*)(void*)>(
					::stbi_load(path.c_str(), &w, &h, &numChannels, 0),
					::stbi_image_free
				);

				std::println("Packing path: {} {} {} {} {}", assetName, ext, w, h, numChannels);

				writer.Write(
					assetName,
					w, h,
					ToTextureFormat(numChannels),
					ToTextureUsage(path),
					{ reinterpret_cast<const std::byte*>(rawData.get()), static_cast<std::size_t>(w * h * numChannels) });
			}
		}

		const auto resourceData = writer.yield();

		Game::Logger::Info("Writing resource {} bytes", resourceData.size());

		std::ofstream out{ argv[2], std::ios::binary };
		out.write(reinterpret_cast<const char*>(resourceData.data()), resourceData.size());
	}
	catch (Game::Exception& e)
	{
		std::println(std::cerr, "{}", e);
	}

	return 0;
}
