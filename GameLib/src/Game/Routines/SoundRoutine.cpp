#include "SoundRoutine.h"

#include "Utilities/Error.h"
#include "Utilities/Logger.h"
#include "Scheduler/Wait.h"

#include <xaudio2.h>

namespace {

	constexpr auto release = [](auto* obj) { obj->Release(); };

}

namespace Game {

	struct SoundRoutine::implementation
	{
		std::unique_ptr<IXAudio2, decltype(release)> xaudio;
	};

	SoundRoutine::SoundRoutine(MessageBus& bus, Scheduler& scheduler)
		: Routine(bus, {})
		, m_Impl{ std::make_unique<implementation>() }
		, m_Scheduler{ scheduler }
	{
		Ensure(XAudio2Create(std::out_ptr(m_Impl->xaudio), 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK, "Failed to create xaudio2");
	}

	SoundRoutine::~SoundRoutine() = default;

	Task SoundRoutine::CreateTask()
	{
		while (m_State != GameState::EXITING)
		{
			co_await Wait{ m_Scheduler, 1u };
		}
	}

}
