#pragma once

#include "Utilities/StringMap.h"
#include "Utilities/Error.h"

#include <string_view>
#include <ranges>
#include <tuple>

namespace Game {

	class Mesh;
	class Texture;
	class Material;

	template <class... T>
	class ResourceCache
	{
	public:
		template <class U, class... Args>
		U* Insert(std::string_view name, Args&&... args)
		{
			auto& map = std::get<StringMap<U>>(m_Maps);

			Expect(!map.contains(name), "{} already exists", name);
			const auto [iter, inserted] = map.emplace(std::make_pair(name, U{ std::forward<Args>(args)... }));

			Expect(inserted, "Insertion failed as key exists");

			return std::addressof(iter->second);
		}

		/*
		* Get an object.
		* 
		* @param name Name of object to get. (undefined behaviour if it doesn't exist)
		* 
		* @returns Pointer to requested object.
		*/
		template <class U>
		const U* Get(std::string_view name)
		{
			const auto& map = std::get<StringMap<U>>(m_Maps);

			const auto object = map.find(name);
			Expect(object != std::ranges::end(map), "{} does not exist", name);

			return std::addressof(object->second);
		}

	private:
		std::tuple<StringMap<T>...> m_Maps;
	};

	using DefaultCache = ResourceCache<Mesh, Material, Texture>;

}
