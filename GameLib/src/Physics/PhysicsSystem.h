#pragma once

#include "Math/Transform.h"
#include "DebugRenderer.h"
#include "CharacterController.h"
#include "RigidBody.h"

#include <memory>
#include <concepts>

namespace Game {

	template<class T>
	concept IsShape = std::derived_from<T, Shape>;

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

		template<IsShape T, class ...Args>
		T* CreateShape(Args&& ...args)
		{
			auto& shape = m_Shapes.emplace_back(std::make_unique<T>(std::forward<Args>(args)..., PassKey<PhysicsSystem>{}));
			return static_cast<T*>(shape.get());
		}

		RigidBody CreateRigidBody(const Shape& shape, const vec3& position, RigidBodyType type) const;

		CharacterController& GetCharacterController() const;

		std::vector<const Shape*> QueryCollisions(const Shape* shape, const Transform& transform);

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_Impl;

		std::vector<std::unique_ptr<Shape>> m_Shapes;
	};

}
