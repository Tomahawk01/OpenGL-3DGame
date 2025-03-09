#pragma once

#include "DebugRenderer.h"
#include "CharacterController.h"
#include "RigidBody.h"

#include <memory>

namespace Game {

	enum class PhysicsLayer
	{
		NON_MOVING,
		MOVING,

		MAX_LAYER
	};

	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Update();

		const DebugRenderer& Debug_Renderer() const;

		template<class T, class ...Args>
		T CreateShape(Args&& ...args) const
		{
			return T{ std::forward<Args>(args)..., PassKey<PhysicsSystem>{} };
		}

		RigidBody CreateRigidBody(const Shape& shape, const vec3& position, RigidBodyType type) const;

		CharacterController& GetCharacterController() const;

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_Impl;
	};

}
