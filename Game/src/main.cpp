#include "Utilities/Exception.h"
#include "Utilities/Logger.h"
#include "UI/DebugUI.h"
#include "Core/Window.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/ResourceLoader.h"
#include "Renderer/Texture.h"
#include "Renderer/Sampler.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/MeshLoader.h"
#include "Renderer/Camera.h"
#include "Renderer/WireframeRenderer.h"
#include "TLV/TLVReader.h"

#include "Game/Chain.h"
#include "Game/AABB.h"
#include "Game/FrustumPlane.h"

#include <iostream>
#include <print>
#include <numbers>
#include <cmath>
#include <type_traits>
#include <ranges>
#include <unordered_map>
#include <random>

namespace {

	bool IntersectsFrustum(const Game::AABB& aabb, const std::array<Game::FrustumPlane, 6u>& planes)
	{
		for (const auto& plane : planes)
		{
			Game::vec3 positiveVertex = aabb.min;
			if (plane.normal.x >= 0)
				positiveVertex.x = aabb.max.x;
			if (plane.normal.y >= 0)
				positiveVertex.y = aabb.max.y;
			if (plane.normal.z >= 0)
				positiveVertex.z = aabb.max.z;

			if (Game::vec3::Dot(plane.normal, positiveVertex) + plane.distance > 0.0f)
				return false;
		}

		return true;
	}

	struct GameTransformState
	{
		const Game::Camera& camera;
		Game::AABB aabb;
		Game::vec3 lastCameraPos;
	};

	constexpr auto CameraDelta = [](const Game::vec3& in, const GameTransformState& state) -> Game::TransformerResult
	{
		return { in + (state.camera.GetPosition() - state.lastCameraPos) };
	};

	constexpr auto Invert = [](const Game::vec3& in, const GameTransformState&) -> Game::TransformerResult
	{
		return { -in };
	};

	constexpr auto CheckVisible = [](const Game::vec3& in, const GameTransformState& state) -> Game::TransformerResult
	{
		const auto planes = state.camera.FrustumPlanes();
		return { in, !IntersectsFrustum(state.aabb, planes) };
	};

	struct TransformedEntity
	{
		Game::Entity entity;
		Game::AABB boundingBox;
		std::unique_ptr<Game::ChainBase<GameTransformState>> transformerChain;
	};

};

int main(int argc, char** argv)
{
	Game::Logger::Info("Starting Game...");

	try
	{
		Game::Ensure(argc == 2, "game.exe <root_path>");

		Game::Window window{ 1280u, 720u, 640u, 360u };

		Game::Camera camera{
			{0.0f, 8.0f, 40.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()),
			static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};

		Game::ResourceLoader resourceLoader{ argv[1] };
		Game::MeshLoader meshLoader{ resourceLoader };

		const Game::File tlvFile{ resourceLoader.Load("resources") };
		const Game::TLVReader reader{ tlvFile.AsData() };
		
		Game::Sampler sampler{};
		Game::Texture albedoTex{ reader, "barrel_Albedo", &sampler };
		Game::Texture specMap{ reader, "barrel_Specular", &sampler };
		Game::Texture normalMap{ reader, "barrel_Normal", &sampler };

		Game::Logger::Info("Textures loaded successfully");

		const Game::Texture* textures[]{ &albedoTex, &specMap, &normalMap };

		const Game::File basicVertFile{ resourceLoader.Load("shaders/basic.vert") };
		const Game::File basicFragFile{ resourceLoader.Load("shaders/basic.frag") };
		const Game::File checkerboardFragFile{ resourceLoader.Load("shaders/checkerboard.frag") };

		const Game::Shader vertexShader{ basicVertFile.AsString(), Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ basicFragFile.AsString(), Game::ShaderType::FRAGMENT };
		const Game::Shader checkerboardShader{ checkerboardFragFile.AsString(), Game::ShaderType::FRAGMENT };
		Game::Material material{ vertexShader, fragmentShader };
		Game::Material checkerboardMaterial{ vertexShader, checkerboardShader };
		const Game::Mesh mesh{ reader, "Barrel" };

		const Game::Renderer renderer{ resourceLoader, meshLoader, window.GetWidth(), window.GetHeight() };

		const Game::Texture floorTexture{
			Game::TextureDescription{
				.width = 1u,
				.height = 1u,
				.format = Game::TextureFormat::RGB,
				.usage = Game::TextureUsage::SRGB,
				.data = { static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff) }
		}, &sampler };
		const Game::Texture* floorTextures[] = { &floorTexture, &floorTexture };
		const Game::Mesh floorMesh{ meshLoader.Cube() };
		Game::Entity floorEntity{ &floorMesh, &checkerboardMaterial, {0.0f, -2.0f, 0.0f}, {100.0f, 1.0f, 100.0f}, floorTextures };

		std::vector<TransformedEntity> entities{};
		entities.emplace_back(
			Game::Entity{ &mesh, &material, {}, {1.0f}, textures },
			Game::AABB{ {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f} },
			std::make_unique<Game::Chain<GameTransformState>>());
		entities.emplace_back(
			Game::Entity{ &mesh, &material, {5.0f, 0.0f, 0.0f}, {1.0f}, textures },
			Game::AABB{ {3.0f, -1.0f, -1.0f}, {5.0f, 1.0f, 1.0f} },
			std::make_unique<Game::Chain<GameTransformState, CheckVisible, CameraDelta>>());
		entities.emplace_back(
			Game::Entity{ &mesh, &material, {-5.0f, 0.0f, 0.0f}, {1.0f}, textures },
			Game::AABB{ {-6.0f, -1.0f, -1.0f}, {-4.0f, 1.0f, 1.0f} },
			std::make_unique<Game::Chain<GameTransformState, CheckVisible, CameraDelta, Invert>>());

		Game::Scene scene{
			.entities = entities | std::views::transform([](const auto& e) { return std::addressof(e.entity); }) | std::ranges::to<std::vector>(),
			.ambient = {0.3f, 0.3f, 0.3f},
			.directionalLight = {.direction = {-1.0f, -1.0f, -1.0f}, .color = {0.5f, 0.5f, 0.5f}},
			.pointLights = {
				{.position = {5.0f, 5.0f, 0.0f},
				.color = {1.0f, 0.0f, 0.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f },

				{.position = {-5.0f, 5.0f, 0.0f},
				.color = {0.0f, 1.0f, 0.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f },

				{.position = {-5.0f, 5.0f, 0.0f},
				.color = {0.0f, 0.0f, 1.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f }
			},
			.debugLines = {}
		};

		scene.entities.push_back(&floorEntity);

		Game::CubeMap skybox{
			reader,
			{{
				"right",
				"left",
				"top",
				"bottom",
				"front",
				"back"
			}}
		};
		Game::Sampler skyboxSampler{};

		std::unordered_map<Game::Key, bool> keyState{};

		float gamma = 2.2f;

		bool showDebug = false;
		const Game::DebugUI debugUI{ window.GetNativeHandle(), scene, camera, gamma };

		GameTransformState state{ camera, {}, camera.GetPosition() };

		Game::WireframeRenderer wireframeRenderer{};

		bool running = true;
		while (running)
		{
			auto event = window.PollEvent();
			while (event && running)
			{
				std::visit([&](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;

					if constexpr (std::same_as<T, Game::StopEvent>)
					{
						running = false;
					}
					else if constexpr (std::same_as<T, Game::KeyEvent>)
					{
						if (arg.GetKey() == Game::Key::ESC)
						{
							running = false;
						}
						
						keyState[arg.GetKey()] = arg.GetState() == Game::KeyState::DOWN;

						if (arg.GetKey() == Game::Key::F1 && arg.GetState() == Game::KeyState::UP)
						{
							showDebug = !showDebug;
						}
					}
					else if constexpr (std::same_as<T, Game::MouseEvent>)
					{
						if (!showDebug)
						{
							static constexpr float sensitivity{ 0.002f };
							const float deltaX = arg.GetDeltaX() * sensitivity;
							const float deltaY = arg.GetDeltaY() * sensitivity;

							camera.AddYaw(deltaX);
							camera.AddPitch(-deltaY);
						}
					}
					else if constexpr (std::same_as<T, Game::MouseButtonEvent>)
					{
						debugUI.AddMouseEvent(arg);
					}
				}, *event);
				event = window.PollEvent();
			}

			Game::vec3 walkDirection{ 0.0f, 0.0f, 0.0f };

			if (keyState[Game::Key::D])
			{
				walkDirection += camera.Right();
			}
			if (keyState[Game::Key::A])
			{
				walkDirection -= camera.Right();
			}
			if (keyState[Game::Key::W])
			{
				walkDirection += camera.GetDirection();
			}
			if (keyState[Game::Key::S])
			{
				walkDirection -= camera.GetDirection();
			}

			walkDirection.y = 0.0f;
			
			const float speed = 0.5f;
			const Game::vec3 velocity = Game::vec3::Normalize(walkDirection) * speed;
			camera.Translate(velocity);

			for (const auto& [transformedEntity, light] : std::views::zip(entities, scene.pointLights))
			{
				auto& [entity, aabb, transformer] = transformedEntity;

				state.aabb = aabb;
				const Game::vec3 entityDelta = transformer->Go({}, state);
				entity.Translate(entityDelta);
				aabb.min += entityDelta;
				aabb.max += entityDelta;
				
				wireframeRenderer.Draw(aabb);

				const auto position = entity.GetPosition();
				light.position = { position.x, 5.0f, position.z };
			}

			/*const std::array<Game::FrustumPlane, 6u> planes{{
				{ .normal = {0.0f, 0.0f, -1.0f}, .distance = 1.0f },
				{ .normal = {0.0f, 0.0f, 1.0f}, .distance = 1.0f },
				{ .normal = {1.0f, 0.0f, 0.0f}, .distance = 1.0f },
				{ .normal = {-1.0f, 0.0f, 0.0f}, .distance = 1.0f },
				{ .normal = {0.0f, 1.0f, 0.0f}, .distance = 1.0f },
				{ .normal = {0.0f, -1.0f, 0.0f}, .distance = 1.0f }
			}};*/
			wireframeRenderer.Draw(camera);

			scene.debugLines = { wireframeRenderer.yield() };

			renderer.Render(camera, scene, skybox, skyboxSampler, gamma);

			if (showDebug)
				debugUI.Render();

			window.Swap();

			state.lastCameraPos = camera.GetPosition();
		}
	}
	catch (const Game::Exception& err)
	{
		std::println(std::cerr, "{}", err);
	}
	catch (...)
	{
		std::println(std::cerr, "Unknown exception");
	}

	return 0;
}
