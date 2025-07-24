#pragma once

#include "Messaging/MessageBus.h"
#include "Messaging/Subscriber.h"
#include "Messaging/AutoSubscribe.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"
#include "TLV/TLVReader.h"
#include "Renderer/Renderer.h"
#include "Renderer/WireframeRenderer.h"
#include "Renderer/MeshLoader.h"

namespace Game {

	class LevelRoutine;

	class RenderRoutine : public Subscriber
	{
	public:
		RenderRoutine(const LevelRoutine& levelRoutine, const Window& window, MessageBus& bus, Scheduler& scheduler, MeshLoader& meshLoader, const TLVReader& reader);

		RenderRoutine(const RenderRoutine&) = delete;
		RenderRoutine& operator=(const RenderRoutine&) = delete;
		RenderRoutine(RenderRoutine&&) = default;

		Task CreateTask();

		void HandleQuit() override;

	private:
		const LevelRoutine& m_LevelRoutine;
		const Window& m_Window;
		MessageBus& m_Bus;
		Scheduler& m_Scheduler;
		Renderer m_Renderer;
		WireframeRenderer m_WireframeRenderer;
		bool m_Running;
		AutoSubscribe m_AutoSub;
	};

}
