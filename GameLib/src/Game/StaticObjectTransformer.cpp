#include "StaticObjectTransformer.h"

namespace Game {

	StaticObjectTransformer::StaticObjectTransformer(const vec3& position)
		: m_Position(position)
	{}

	void StaticObjectTransformer::Update()
	{}

	vec3 StaticObjectTransformer::Position() const
	{
		return m_Position;
	}

}
