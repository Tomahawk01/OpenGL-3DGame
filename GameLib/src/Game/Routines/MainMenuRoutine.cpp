#include "MainMenuRoutine.h"

#include "Scheduler/Wait.h"

#include <numbers>

using namespace std::literals;

namespace {

	Game::Camera CreateCamera(const Game::Window& window)
	{
		return {
			{0.0f, 5.0f, 20.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()),
			static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};
	}

}

namespace Game {

	MainMenuRoutine::MainMenuRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache, const TLVReader& reader)
		: Routine(bus, { MessageType::KEY_PRESS })
		, m_Window{ window }
		, m_Scheduler{ scheduler }
		, m_ResourceCache{ resourceCache }
		, m_Reader{ reader }
		, m_Camera{ CreateCamera(window) }
		, m_Scene{}
	{
		m_Window.SetTitle("Barrel Game");

		m_Scene = Scene{
			.entities = {},
			.ambient = {0.5f, 0.5f, 0.5f},
			.directionalLight = {},
			.pointLights = {},
			.debugLines = {},
			.skybox = nullptr,
			.skyboxSampler = resourceCache.Get<Sampler>("sky_box"),
			.labels = {},
			.effects = {
				.hdr = true,
				.grayScale = false,
				.blur = false
			}
		};

		m_Bus.PostNewCamera(&m_Camera);
		m_Bus.PostNewScene(&m_Scene);
	}

	Task MainMenuRoutine::CreateTask()
	{
		while (m_State != GameState::EXITING)
		{
			if (m_State != GameState::MAIN_MENU)
			{
				co_await Wait{ m_Scheduler, GameState::MAIN_MENU };
			}

			co_await Wait{ m_Scheduler, 16ms };
		}
	}

	void MainMenuRoutine::HandleKeyPress(const KeyEvent& event)
	{
		if (m_State == GameState::MAIN_MENU)
		{
			if (event.GetState() == KeyState::DOWN)
			{
				if (event.GetKey() == Key::ESC)
				{
					m_Bus.PostStateChange(GameState::EXITING);
				}
				else
				{
					m_Bus.PostStateChange(GameState::RUNNING);
				}
			}
		}
	}

}
