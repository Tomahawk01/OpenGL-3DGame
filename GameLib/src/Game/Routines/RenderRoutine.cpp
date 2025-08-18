#include "RenderRoutine.h"

#include "Utilities/Error.h"
#include "Scheduler/Wait.h"

namespace Game {

	RenderRoutine::RenderRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler, MeshLoader& meshLoader, const TLVReader& reader)
		: Routine(bus, {MessageType::STATE_CHANGE, MessageType::NEW_CAMERA, MessageType::NEW_SCENE})
		, m_Window(window)
		, m_Scheduler(scheduler)
		, m_Renderer{ reader, meshLoader, m_Window.GetWidth(), m_Window.GetHeight() }
		, m_WireframeRenderer{}
		, m_Scene{}
		, m_Camera{}
	{}

	Task RenderRoutine::CreateTask()
	{
		float gamma = 2.2f;

		while (m_State != GameState::EXITING)
		{
			Expect(m_Scene, "Scene cannot be null");
			Expect(m_Camera, "Camera cannot be null");

			m_WireframeRenderer.Draw(*m_Camera);
			m_Renderer.Render(*m_Camera, *m_Scene, gamma);

			m_Window.Swap();

			co_await Wait{ m_Scheduler, 1u };
		}
	}

	void RenderRoutine::HandleNewCamera(Camera* camera)
	{
		m_Camera = camera;
	}

	void RenderRoutine::HandleNewScene(Scene* scene)
	{
		m_Scene = scene;
	}

}
