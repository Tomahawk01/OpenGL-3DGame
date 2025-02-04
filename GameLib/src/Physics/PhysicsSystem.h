#pragma once

#include <memory>

namespace Game {

	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Update();

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_Impl;
	};

}
