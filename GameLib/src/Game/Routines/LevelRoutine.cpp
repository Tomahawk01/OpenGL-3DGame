#include "LevelRoutine.h"

#include "Scheduler/Wait.h"
#include "Physics/BoxShape.h"

#include <numbers>
#include <filesystem>

using namespace std::literals;

namespace {

	bool IntersectsFrustum(const Game::TransformedShape& boundingBox, const std::array<Game::FrustumPlane, 6u>& planes)
	{
		Game::Expect(boundingBox.GetShape()->GetType() == Game::ShapeType::BOX, "This is not a box");

		const auto* boxShape = static_cast<const Game::BoxShape*>(boundingBox.GetShape());

		const auto position = boundingBox.GetTransform().Position;
		const auto min = position - boxShape->GetDimensions();
		const auto max = position + boxShape->GetDimensions();

		for (const auto& plane : planes)
		{
			Game::vec3 positiveVertex = min;
			if (plane.normal.x >= 0)
				positiveVertex.x = max.x;
			if (plane.normal.y >= 0)
				positiveVertex.y = max.y;
			if (plane.normal.z >= 0)
				positiveVertex.z = max.z;

			if (Game::vec3::Dot(plane.normal, positiveVertex) + plane.distance < 0.0f)
				return false;
		}

		return true;
	}

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

	std::vector<Game::ScriptLoader> GetLevelLoaders(const Game::TLVReader& reader)
	{
		std::vector<Game::ScriptLoader> customLevelNames{};

		if (std::filesystem::exists("levels"))
		{
			auto files = std::filesystem::directory_iterator{ "levels" } | std::ranges::to<std::vector>();
			std::ranges::sort(files, [](const auto& a, const auto& b) { return a.path() < b.path(); });

			customLevelNames = files | std::views::transform(
				[](const auto& e)
				{
					return e.path().string();
				})
				| std::views::filter([](const auto& e) { return e.ends_with(".lua"); })
				| std::views::transform([](const auto& e) { return Game::ScriptLoader{ e }; })
				| std::ranges::to<std::vector>();
		}

		const auto builtinLevelNames = reader | std::views::filter(
			[](const auto& entry)
			{
				if (entry.Type() == Game::TLVType::TEXT_FILE)
				{
					const auto textFile = entry.textFileValue();
					return textFile.Name.ends_with(".lua");
				}
				return false;
			})
			| std::views::transform([&reader](const auto& entry) { return Game::ScriptLoader{ entry.textFileValue().Name, reader }; })
			| std::ranges::to<std::vector>();

		std::vector<Game::ScriptLoader> levelNames{};
		levelNames.insert_range(std::ranges::end(levelNames), customLevelNames);
		levelNames.insert_range(std::ranges::end(levelNames), builtinLevelNames);

		return levelNames;
	}

}

namespace Game {

	LevelRoutine::LevelRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache, const TLVReader& reader)
		: m_Window{ window }
		, m_Bus{ bus }
		, m_Scheduler{ scheduler }
		, m_Player{ m_Bus, CreateCamera(window) }
		, m_ResourceCache{ resourceCache }
		, m_Reader{ reader }
		, m_LevelNum{ 0u }
		, m_LevelLoaders{ GetLevelLoaders(m_Reader) }
		, m_Level{ std::make_unique<LuaLevel>(m_LevelLoaders[m_LevelNum], resourceCache, reader, m_Player, m_Bus) }
		, m_Running{ true }
		, m_AutoSub{ m_Bus, {MessageType::LEVEL_COMPLETE, MessageType::QUIT}, this }
	{
		m_Window.SetTitle(m_LevelLoaders[m_LevelNum].GetName());
	}

	Task LevelRoutine::CreateTask()
	{
		auto currentLevel = m_LevelNum;

		while (m_Running)
		{
			if (currentLevel != m_LevelNum)
			{
				m_Player.Restart();
				m_Level.reset(nullptr);
				m_Level = std::make_unique<LuaLevel>(m_LevelLoaders[m_LevelNum], m_ResourceCache, m_Reader, m_Player, m_Bus);
				currentLevel = m_LevelNum;

				m_Window.SetTitle(m_LevelLoaders[m_LevelNum].GetName());
			}

			Expect(m_Level, "Level cannot be nullptr");

			m_Player.Update();
			m_Level->Update(m_Player);

			for (auto& entity : m_Level->GetScene().entities)
			{
				entity->SetVisibility(IntersectsFrustum(entity->GetBoundingBox(), m_Player.GetCamera().FrustumPlanes()));
			}

			co_await Wait{ m_Scheduler, 1u };
		}
	}

	const Player& LevelRoutine::GetPlayer() const
	{
		return m_Player;
	}

	LuaLevel& LevelRoutine::GetLevel() const
	{
		return *m_Level;
	}

	void LevelRoutine::HandleLevelComplete(std::string_view levelName)
	{
		Logger::Info("Level complete: {}", levelName);
		m_LevelNum = (m_LevelNum + 1) % m_LevelLoaders.size();
	}

	void LevelRoutine::HandleQuit()
	{
		m_Running = false;
	}

}
