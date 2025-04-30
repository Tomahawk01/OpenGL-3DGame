#pragma once

#include "Core/Scene.h"

namespace Game {

	class Player;

	class Level
	{
	public:
		virtual ~Level() = default;

		virtual void Update(const Player& player) = 0;

		inline Scene& GetScene() { return m_Scene; }

	protected:
		Scene m_Scene;
	};

}
