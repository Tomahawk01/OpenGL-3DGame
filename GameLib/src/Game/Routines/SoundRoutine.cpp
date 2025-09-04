#include "SoundRoutine.h"

#include "Core/File.h"
#include "Utilities/Error.h"
#include "Utilities/Logger.h"
#include "Utilities/Formatter.h"
#include "Scheduler/Wait.h"

#include <cstddef>
#include <ranges>
#include <span>
#include <algorithm>
#include <utility>

#include <xaudio2.h>

namespace {

	constexpr auto release = [](auto* obj) { obj->Release(); };
	constexpr auto destroyVoice = [](auto* obj) { obj->DestroyVoice(); };

}

namespace Game {

	struct SoundRoutine::Implementation
	{
		std::unique_ptr<IXAudio2, decltype(release)> xaudio;
		std::unique_ptr<IXAudio2MasteringVoice, decltype(destroyVoice)> masteringVoice;
		std::unique_ptr<IXAudio2SourceVoice, decltype(destroyVoice)> sourceVoice;
		std::vector<std::byte> soundData;
	};

	SoundRoutine::SoundRoutine(MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache)
		: Routine(bus, {})
		, m_Impl{ std::make_unique<Implementation>() }
		, m_Scheduler{ scheduler }
		, m_ResourceCache{ resourceCache }
	{
		Ensure(XAudio2Create(std::out_ptr(m_Impl->xaudio), 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK, "Failed to create xaudio2");
		Ensure(m_Impl->xaudio->CreateMasteringVoice(std::out_ptr(m_Impl->masteringVoice), XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0u, nullptr, nullptr) == S_OK, "Failed to create mastering voice");

		const auto* mainTheme = resourceCache.Get<SoundData>("main_theme");

		WAVEFORMATEXTENSIBLE wfx{};
		std::memcpy(&wfx, mainTheme->format.data(), std::min(sizeof(wfx), mainTheme->format.size_bytes()));

		m_Impl->soundData = mainTheme->data | std::ranges::to<std::vector>();

		const XAUDIO2_BUFFER xaudioBuffer{
			.Flags = XAUDIO2_END_OF_STREAM,
			.AudioBytes = static_cast<UINT32>(mainTheme->data.size_bytes()),
			.pAudioData = reinterpret_cast<const BYTE*>(m_Impl->soundData.data()),
			.PlayBegin = 0,
			.PlayLength = 0,
			.LoopBegin = 0,
			.LoopLength = 0,
			.LoopCount = XAUDIO2_LOOP_INFINITE,
			.pContext = nullptr
		};

		m_Impl->xaudio->CreateSourceVoice(std::out_ptr(m_Impl->sourceVoice), reinterpret_cast<WAVEFORMATEX*>(&wfx));

		Ensure(m_Impl->sourceVoice->SubmitSourceBuffer(&xaudioBuffer) == S_OK, "Failed to submit sound buffer");
		Ensure(m_Impl->sourceVoice->Start(0) == S_OK, "Failed to start sound");
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
