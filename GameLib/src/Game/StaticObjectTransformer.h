#pragma once

#include "ObjectTransformer.h"

namespace Game {

	class StaticObjectTransformer : public ObjectTransformer
	{
	public:
		StaticObjectTransformer(const vec3& position);
		~StaticObjectTransformer() override = default;

		void Update() override;
		vec3 Position() const override;

	private:
		vec3 m_Position;
	};

}
