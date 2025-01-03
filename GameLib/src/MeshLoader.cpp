#include "MeshLoader.h"

#include "Math/Vector3.h"
#include "Utilities/Error.h"
#include "Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/cimport.h>

#include <ranges>

namespace {

	template<class ...Args>
	std::vector<Game::VertexData> Vertices(Args&& ...args)
	{
		return std::views::zip_transform(
			[]<class ...A>(A&& ...a) { return Game::VertexData{ std::forward<A>(a)... }; },
			std::forward<Args>(args)...) |
			std::ranges::to<std::vector>();
	}

}

namespace Game {

	MeshLoader::MeshLoader(ResourceLoader& resourceLoader)
		: m_ResourceLoader(resourceLoader)
	{}

	MeshData MeshLoader::Cube()
	{
		const auto loaded = m_LoadedMeshes.find("cube");
		if (loaded != std::ranges::cend(m_LoadedMeshes))
		{
			return {
				.vertices = loaded->second.vertices,
				.indices = loaded->second.indices
			};
		}

		const vec3 positions[] = {
			{-0.5f, -0.5f, -0.5f},
			{0.5f,  -0.5f, -0.5f},
			{0.5f,   0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},
			{-0.5f, -0.5f,  0.5f},
			{0.5f,  -0.5f,  0.5f},
			{0.5f,   0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{-0.5f, -0.5f,  0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},
			{-0.5f,  0.5f,  0.5f},
			{0.5f,  -0.5f, -0.5f},
			{0.5f,  -0.5f,  0.5f},
			{0.5f,   0.5f,  0.5f},
			{0.5f,   0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},
			{0.5f,   0.5f, -0.5f},
			{0.5f,   0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{-0.5f, -0.5f,  0.5f},
			{0.5f,  -0.5f,  0.5f},
			{0.5f,  -0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f}
		};

		const vec3 normals[] = {
			{0.0f,  0.0f, -1.0f},
			{0.0f,  0.0f, -1.0f},
			{0.0f,  0.0f, -1.0f},
			{0.0f,  0.0f, -1.0f},
			{0.0f,  0.0f,  1.0f},
			{0.0f,  0.0f,  1.0f},
			{0.0f,  0.0f,  1.0f},
			{0.0f,  0.0f,  1.0f},
			{-1.0f, 0.0f,  0.0f},
			{-1.0f, 0.0f,  0.0f},
			{-1.0f, 0.0f,  0.0f},
			{-1.0f, 0.0f,  0.0f},
			{1.0f,  0.0f,  0.0f},
			{1.0f,  0.0f,  0.0f},
			{1.0f,  0.0f,  0.0f},
			{1.0f,  0.0f,  0.0f},
			{0.0f,  1.0f,  0.0f},
			{0.0f,  1.0f,  0.0f},
			{0.0f,  1.0f,  0.0f},
			{0.0f,  1.0f,  0.0f},
			{0.0f, -1.0f,  0.0f},
			{0.0f, -1.0f,  0.0f},
			{0.0f, -1.0f,  0.0f},
			{0.0f, -1.0f,  0.0f}
		};

		const UV uvs[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		const std::vector<std::uint32_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20
		};

		const auto newItem = m_LoadedMeshes.emplace("cube", LoadedMeshData{ Vertices(positions, normals, normals, uvs), std::move(indices) });

		return {.vertices = newItem.first->second.vertices, .indices = newItem.first->second.indices };
	}

	MeshData MeshLoader::Sprite()
	{
		const auto loaded = m_LoadedMeshes.find("sprite");
		if (loaded != std::ranges::cend(m_LoadedMeshes))
		{
			return {
				.vertices = loaded->second.vertices,
				.indices = loaded->second.indices
			};
		}

		const vec3 positions[] = {
			{-1.0f, 1.0f, 0.0f},
			{-1.0f, -1.0f, 0.0f},
			{1.0f, -1.0f, 0.0f},
			{1.0f, 1.0f, 0.0f}
		};

		const UV uvs[] = {
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f}			
		};

		const std::vector<std::uint32_t> indices = {
			0, 1, 2, 0, 2, 3
		};

		const auto newItem = m_LoadedMeshes.emplace("sprite", LoadedMeshData{ Vertices(positions, positions, positions, uvs), std::move(indices) });

		return { .vertices = newItem.first->second.vertices, .indices = newItem.first->second.indices };
	}

	MeshData MeshLoader::Load(std::string_view meshFile, std::string_view modelName)
	{
		auto stream = ::aiGetPredefinedLogStream(::aiDefaultLogStream_STDOUT, NULL);
		::aiAttachLogStream(&stream);

		::aiEnableVerboseLogging(true);

		const auto modelFileData = m_ResourceLoader.LoadBinary(meshFile);
		Ensure(!modelFileData.empty(), "No loaded data");

		::Assimp::Importer importer{};
		const auto* scene = importer.ReadFileFromMemory(
			modelFileData.data(),
			modelFileData.size(),
			::aiProcess_Triangulate | ::aiProcess_FlipUVs | ::aiProcess_CalcTangentSpace);

		Ensure(scene != nullptr && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE), "Failed to load model {} {}", meshFile, modelName);

		const std::span<::aiMesh*> loadedMeshes{ scene->mMeshes, scene->mMeshes + scene->mNumMeshes };

		Logger::Trace("Found {} meshes", loadedMeshes.size());

		for (const auto* mesh : loadedMeshes)
		{
			const auto loaded = m_LoadedMeshes.find(mesh->mName.C_Str());
			if (loaded != std::ranges::cend(m_LoadedMeshes))
				continue;

			const auto toVector3 = [](const ::aiVector3D& v) { return vec3{ v.x, v.y, v.z }; };
			const auto positions = std::span<::aiVector3D>{ mesh->mVertices, mesh->mVertices + mesh->mNumVertices } | std::views::transform(toVector3);
			const auto normals = std::span<::aiVector3D>{ mesh->mNormals, mesh->mNormals + mesh->mNumVertices } | std::views::transform(toVector3);

			std::vector<UV> uvs{};
			std::vector<vec3> tangents{};
			for (auto i = 0u; i < mesh->mNumVertices; i++)
			{
				uvs.push_back({ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
				tangents.push_back({ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
			}

			std::vector<std::uint32_t> indices{};
			for (auto i = 0u; i < mesh->mNumFaces; i++)
			{
				const auto& face = mesh->mFaces[i];
				for (auto j = 0u; j < face.mNumIndices; j++)
				{
					indices.push_back(face.mIndices[j]);
				}
			}

			m_LoadedMeshes.emplace(mesh->mName.C_Str(), LoadedMeshData{Vertices(positions, normals, tangents, uvs), std::move(indices)});
		}

		const auto loaded = m_LoadedMeshes.find(modelName);
		if (loaded != std::ranges::cend(m_LoadedMeshes))
		{
			return {
				.vertices = loaded->second.vertices,
				.indices = loaded->second.indices
			};
		}

		Ensure(false, "Failed to load {} from {}", modelName, meshFile);
		return {};
	}

}
