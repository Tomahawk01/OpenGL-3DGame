#pragma once

#include "Scheduler/Scheduler.h"
#include "Core/Window.h"
#include "TLV/TLVReader.h"
#include "Renderer/Renderer.h"
#include "Renderer/WireframeRenderer.h"
#include "Game/Routines/Routine.h"

namespace Game {

	class LevelRoutine;

	class RenderRoutine : public Routine
	{
	public:
		RenderRoutine(const LevelRoutine& levelRoutine, const Window& window, MessageBus& bus, Scheduler& scheduler, MeshLoader& meshLoader, const TLVReader& reader);

		RenderRoutine(const RenderRoutine&) = delete;
		RenderRoutine& operator=(const RenderRoutine&) = delete;
		RenderRoutine(RenderRoutine&&) = default;

		Task CreateTask();

	private:
		const LevelRoutine& m_LevelRoutine;
		const Window& m_Window;
		Scheduler& m_Scheduler;
		Renderer m_Renderer;
		WireframeRenderer m_WireframeRenderer;
	};

}
