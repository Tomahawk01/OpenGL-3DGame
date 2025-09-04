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

	std::span<const std::byte> ParseChunk(std::span<const std::byte> source, std::span<const std::byte> controlCode)
	{
		const auto chunkHeader = std::ranges::search(source, controlCode);
		Game::Ensure(!chunkHeader.empty(), "Could not find control code in chunk");

		const auto headerOffset = std::ranges::distance(std::ranges::cbegin(source), std::ranges::cbegin(chunkHeader));

		uint32_t chunkSize{};
		std::memcpy(&chunkSize, source.data() + headerOffset + chunkHeader.size(), sizeof(chunkSize));

		Game::Logger::Trace("headerOffset: {:x} | chunkSize: {:x}", headerOffset, chunkSize);

		return { source.data() + headerOffset + chunkHeader.size() + sizeof(chunkSize), chunkSize };
	}

}

namespace Game {

	struct SoundRoutine::implementation
	{
		std::unique_ptr<IXAudio2, decltype(release)> xaudio;
		std::unique_ptr<IXAudio2MasteringVoice, decltype(destroyVoice)> masteringVoice;
		std::unique_ptr<IXAudio2SourceVoice, decltype(destroyVoice)> sourceVoice;
		std::vector<std::byte> soundData;
	};

	SoundRoutine::SoundRoutine(MessageBus& bus, Scheduler& scheduler)
		: Routine(bus, {})
		, m_Impl{ std::make_unique<implementation>() }
		, m_Scheduler{ scheduler }
	{
		Ensure(XAudio2Create(std::out_ptr(m_Impl->xaudio), 0, XAUDIO2_DEFAULT_PROCESSOR) == S_OK, "Failed to create xaudio2");
		Ensure(m_Impl->xaudio->CreateMasteringVoice(std::out_ptr(m_Impl->masteringVoice), XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0u, nullptr, nullptr) == S_OK, "Failed to create mastering voice");

		const File audioFile{ "assets/main_theme.wav" };
		const auto data = audioFile.AsData();

		const std::byte riffCC[]{ std::byte{'R'}, std::byte{'I'}, std::byte{'F'}, std::byte{'F'} };
		const auto riffChunk = ParseChunk(data, riffCC);

		const std::byte waveCC[]{ std::byte{'W'}, std::byte{'A'}, std::byte{'V'}, std::byte{'E'} };
		ParseChunk(riffChunk, waveCC);

		const std::byte fmtCC[]{ std::byte{'f'}, std::byte{'m'}, std::byte{'t'}, std::byte{' '} };
		const auto fmtChunk = ParseChunk(data, fmtCC);

		WAVEFORMATEXTENSIBLE wfx{};

		std::memcpy(&wfx, fmtChunk.data(), std::min(sizeof(wfx), fmtChunk.size_bytes()));

		const std::byte dataCC[]{ std::byte{'d'}, std::byte{'a'}, std::byte{'t'}, std::byte{'a'} };
		const auto dataChunk = ParseChunk(data, dataCC);

		m_Impl->soundData = dataChunk | std::ranges::to<std::vector>();

		const XAUDIO2_BUFFER xaudioBuffer{
			.Flags = XAUDIO2_END_OF_STREAM,
			.AudioBytes = static_cast<UINT32>(dataChunk.size_bytes()),
			.pAudioData = reinterpret_cast<const BYTE*>(m_Impl->soundData.data())
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
