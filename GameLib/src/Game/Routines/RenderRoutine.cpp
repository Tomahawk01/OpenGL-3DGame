#include "RenderRoutine.h"

#include "LevelRoutine.h"
#include "Scheduler/Wait.h"

namespace Game {

	RenderRoutine::RenderRoutine(const LevelRoutine& levelRoutine, const Window& window, MessageBus& bus, Scheduler& scheduler, MeshLoader& meshLoader, const TLVReader& reader)
		: m_LevelRoutine(levelRoutine)
		, m_Window(window)
		, m_Bus(bus)
		, m_Scheduler(scheduler)
		, m_Renderer{ reader, meshLoader, m_Window.GetWidth(), m_Window.GetHeight() }
		, m_WireframeRenderer{}
		, m_Running{ true }
	{
		m_Bus.Subscribe(MessageType::QUIT, this);
	}

	Task RenderRoutine::CreateTask()
	{
		float gamma = 2.2f;

		while (m_Running)
		{
			m_WireframeRenderer.Draw(m_LevelRoutine.GetPlayer().GetCamera());
			m_Renderer.Render(m_LevelRoutine.GetPlayer().GetCamera(), m_LevelRoutine.GetLevel().GetScene(), gamma);

			m_Window.Swap();

			co_await Wait{ m_Scheduler, 1u };
		}
	}

	void RenderRoutine::HandleQuit()
	{
		m_Running = false;
	}

}
