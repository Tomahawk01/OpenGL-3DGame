#pragma once

#include "Messaging/MessageBus.h"
#include "Messaging/Subscriber.h"
#include "Messaging/AutoSubscribe.h"

#include "Game/GameState.h"

#include <set>

namespace Game {

	class Routine : public Subscriber
	{
	public:
		Routine(MessageBus& bus, std::set<MessageType> types);
		virtual ~Routine() = default;

		virtual void HandleStateChange(GameState state) override;

	protected:
		MessageBus& m_Bus;
		GameState m_State;

	private:
		AutoSubscribe m_AutoSub;
	};

}
