#include "Routine.h"

namespace {

	std::set<Game::MessageType> EnrichTypes(std::set<Game::MessageType> types)
	{
		types.emplace(Game::MessageType::STATE_CHANGE);
		return types;
	}

}

namespace Game {

	Routine::Routine(MessageBus& bus, std::set<MessageType> types)
		: Subscriber()
		, m_Bus{ bus }
		, m_State{ GameState::MAIN_MENU }
		, m_AutoSub{ m_Bus, EnrichTypes(std::move(types)), this }
	{}

	void Routine::HandleStateChange(GameState state)
	{
		m_State = state;
	}

}
