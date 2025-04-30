#pragma once

#include "Utilities/StringMap.h"
#include "Utilities/Error.h"
#include "Renderer/Mesh.h"

#include <string_view>
#include <ranges>

namespace Game {

	class ResourceCache
	{
	public:
		template <class... Args>
		Mesh* InsertMesh(std::string_view name, Args&&... args)
		{
			Expect(!m_Meshes.contains(name), "{} already exists", name);
			const auto [iter, inserted] = m_Meshes.emplace(std::make_pair(name, Mesh{ std::forward<Args>(args)... }));

			Expect(inserted, "Insertion failed as key exists");

			return std::addressof(iter->second);
		}

		/*
		* Get a mesh.
		* 
		* @param name Name of mesh to get (undefined behaviour if it doesn't exist)
		* 
		* @returns Pointer to requested mesh.
		*/
		const Mesh* GetMesh(std::string_view name)
		{
			const auto mesh = m_Meshes.find(name);
			Expect(mesh != std::ranges::end(m_Meshes), "{} does not exist", name);

			return std::addressof(mesh->second);
		}

	private:
		StringMap<Mesh> m_Meshes;
	};

}
