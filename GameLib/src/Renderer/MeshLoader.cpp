#include "MeshLoader.h"

#include "Math/Vector3.h"
#include "Utilities/Error.h"
#include "Utilities/Logger.h"

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

		const std::vector<uint32_t> indices = {
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

		const std::vector<uint32_t> indices = {
			0, 1, 2, 0, 2, 3
		};

		const auto newItem = m_LoadedMeshes.emplace("sprite", LoadedMeshData{ Vertices(positions, positions, positions, uvs), std::move(indices) });

		return { .vertices = newItem.first->second.vertices, .indices = newItem.first->second.indices };
	}

	MeshData MeshLoader::Load(std::string_view meshFile, std::string_view modelName)
	{
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
