#pragma once

#include <concepts>
#include <format>

namespace Game {

	template <class T>
	concept HasToStringMember = requires(T a)
	{
		{ a.to_string() } -> std::convertible_to<std::string>;
	};

	template <class T>
	concept HasToStringFree = requires(T a)
	{
		{ to_string(a) } -> std::convertible_to<std::string>;
	};

	namespace Util {

		struct ToStringCPO
		{
			template <HasToStringMember T>
			std::string operator()(T&& obj) const
			{
				return obj.to_string();
			}

			template <class T> requires (!HasToStringMember<T> && HasToStringFree<T>)
			std::string operator()(T&& obj) const
			{
				return to_string(obj);
			}
		};

		inline constexpr auto to_string = ToStringCPO{};

	}

	template <class T>
	struct Formatter
	{
		constexpr auto parse(std::format_parse_context& ctx)
		{
			return std::ranges::begin(ctx);
		}

		auto format(const T& obj, std::format_context& ctx) const
		{
			return std::format_to(ctx.out(), "{}", Util::to_string(obj));
		}
	};

}

template <class T>
concept CanFormat = requires(T a)
{
	{ Game::Util::to_string(a) } -> std::convertible_to<std::string>;
};

template <CanFormat T>
struct std::formatter<T> : Game::Formatter<T>
{
};
