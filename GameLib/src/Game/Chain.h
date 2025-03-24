#pragma once

#include "Math/Vector3.h"
#include "Renderer/Camera.h"

namespace Game {

	struct TransformerResult
	{
		vec3 result;
		bool stop = false;
	};

	template<class S>
	struct ChainBase
	{
		virtual constexpr vec3 Go(const vec3&, const S&) const
		{
			return {};
		};
	};

	template <class S, auto... T>
	struct Chain final : ChainBase<S>
	{
	};

	template <auto Head, auto... Tail, class S>
	struct Chain<S, Head, Tail...> final : ChainBase<S>
	{
		constexpr vec3 Go(const vec3& in, const S& state) const override
		{
			const auto [result, stop] = Head(in, state);

			return stop ? result : Chain<S, Tail...>{}.Go(result, state);
		}
	};

	template <auto Head, class S>
	struct Chain<S, Head> final : ChainBase<S>
	{
		constexpr vec3 Go(const vec3& in, const S& state) const override
		{
			return Head(in, state).result;
		}
	};

	template <class S>
	struct Chain<S> final : ChainBase<S>
	{
		constexpr vec3 Go(const vec3& in, const S&) const override
		{
			return in;
		}
	};

}
