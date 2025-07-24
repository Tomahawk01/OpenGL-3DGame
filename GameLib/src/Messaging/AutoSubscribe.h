#pragma once

#include "Messaging/MessageBus.h"
#include "Messaging/Subscriber.h"

#include <set>

namespace Game {

	class AutoSubscribe
	{
	public:
		AutoSubscribe(MessageBus& bus, std::set<MessageType> types, Subscriber* subscriber)
			: m_Bus(bus)
			, m_Types(std::move(types))
			, m_Subscriber(subscriber)
		{
			for (const auto type : m_Types)
			{
				m_Bus.Subscribe(type, m_Subscriber);
			}
		}

		~AutoSubscribe()
		{
			for (const auto type : m_Types)
			{
				m_Bus.Unsubscribe(type, m_Subscriber);
			}
		}

	private:
		MessageBus& m_Bus;
		std::set<MessageType> m_Types;
		Subscriber* m_Subscriber;
	};

}
