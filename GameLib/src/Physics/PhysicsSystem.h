#pragma once

#include "DebugRenderer.h"

#include <memory>

namespace Game {

	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Update();

		const DebugRenderer& Debug_Renderer() const;

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_Impl;
	};

}
