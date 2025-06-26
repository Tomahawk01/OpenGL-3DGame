#include "Utilities/Error.h"
#include "Utilities/AutoRelease.h"
#include "Utilities/Logger.h"
#include "TLV/TLVWriter.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/cimport.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <set>
#include <ranges>

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
		Game::Logger::Info("Resource packer");

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

				Game::Logger::Info("Packing path: {} {} {} {} {}", assetName, ext, w, h, numChannels);

				writer.Write(
					assetName,
					w, h,
					ToTextureFormat(numChannels),
					ToTextureUsage(path),
					{ reinterpret_cast<const std::byte*>(rawData.get()), static_cast<size_t>(w * h * numChannels) });
			}
			else if (ext == ".obj")
			{
				auto stream = ::aiGetPredefinedLogStream(::aiDefaultLogStream_STDOUT, NULL);
				::aiAttachLogStream(&stream);

				::Assimp::Importer importer{};
				const auto* scene = importer.ReadFile(path.c_str(), ::aiProcess_Triangulate | ::aiProcess_FlipUVs | ::aiProcess_CalcTangentSpace);
				Game::Ensure(scene != nullptr, std::format("Failed to load model {}", path));

				const std::span<::aiMesh*> loadedMeshes{ scene->mMeshes, scene->mMeshes + scene->mNumMeshes };

				for (const auto* mesh : loadedMeshes)
				{
					Game::Logger::Info("Packing {}", mesh->mName.C_Str());

					const auto toVector3 = [](const ::aiVector3D& v) { return Game::vec3{ v.x, v.y, v.z }; };
					const auto positions = std::span<::aiVector3D>{ mesh->mVertices, mesh->mVertices + mesh->mNumVertices } | std::views::transform(toVector3);
					const auto normals = std::span<::aiVector3D>{ mesh->mNormals, mesh->mNormals + mesh->mNumVertices } | std::views::transform(toVector3);

					std::vector<Game::UV> uvs{};
					std::vector<Game::vec3> tangents{};
					for (auto i = 0u; i < mesh->mNumVertices; i++)
					{
						uvs.push_back({ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
						tangents.push_back({ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
					}

					std::vector<uint32_t> indices{};
					for (auto i = 0u; i < mesh->mNumFaces; i++)
					{
						const auto& face = mesh->mFaces[i];
						for (auto j = 0u; j < face.mNumIndices; j++)
						{
							indices.push_back(face.mIndices[j]);
						}
					}

					const auto vertices = std::views::zip_transform(
						[]<class ...A>(A&& ...a) { return Game::VertexData{ std::forward<A>(a)... }; },
						positions, normals, tangents, uvs) |
						std::ranges::to<std::vector>();

					writer.Write(mesh->mName.C_Str(), vertices, indices);
				}
			}
		}

		const auto resourceData = writer.yield();

		Game::Logger::Info("Writing resource {} bytes", resourceData.size());

		std::ofstream out{ argv[2], std::ios::binary };
		out.write(reinterpret_cast<const char*>(resourceData.data()), resourceData.size());
	}
	catch (Game::Exception& e)
	{
		Game::Logger::Error("{}", e);
	}

	return 0;
}
