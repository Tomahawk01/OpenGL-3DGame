#pragma once

#include "Utilities/PassKey.h"
#include "Renderer/MeshData.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/MeshShape.h>

namespace Game {

	class PhysicsSystem;

	class MeshShape : public Shape
	{
	public:
		MeshShape(MeshData meshData, float meshScale, PassKey<PhysicsSystem>);

		const ::JPH::Shape* GetNativeHandle() const override;

		std::string to_string() const;

	private:
		::JPH::Ref<::JPH::Shape> m_MeshShape;
	};

}
