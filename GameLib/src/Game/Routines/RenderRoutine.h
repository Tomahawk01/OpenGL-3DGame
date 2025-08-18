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
		RenderRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler, MeshLoader& meshLoader, const TLVReader& reader);

		RenderRoutine(const RenderRoutine&) = delete;
		RenderRoutine& operator=(const RenderRoutine&) = delete;
		RenderRoutine(RenderRoutine&&) = default;

		Task CreateTask();

		void HandleNewCamera(Camera* camera) override;
		void HandleNewScene(Scene* scene) override;

	private:
		const Window& m_Window;
		Scheduler& m_Scheduler;
		Renderer m_Renderer;
		WireframeRenderer m_WireframeRenderer;
		Scene* m_Scene;
		Camera* m_Camera;
	};

}
