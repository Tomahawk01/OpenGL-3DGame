#include "PhysicsSystem.h"

#include "Utilities/Logger.h"
#include "Utilities/Error.h"
#include "BoxShape.h"
#include "SphereShape.h"
#include "RigidBody.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/Memory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <cstdarg>
#include <set>
#include <thread>

using namespace JPH::literals;

namespace {

	class SimpleBroadPhaseLayer : public ::JPH::BroadPhaseLayerInterface
	{
	public:
		virtual ::JPH::uint GetNumBroadPhaseLayers() const override { return std::to_underlying(Game::PhysicsLayer::MAX_LAYER); }
		virtual ::JPH::BroadPhaseLayer GetBroadPhaseLayer(::JPH::ObjectLayer layer) const override { return ::JPH::BroadPhaseLayer(static_cast<::JPH::BroadPhaseLayer::Type>(layer)); }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(::JPH::BroadPhaseLayer layer) const override
		{
			Game::PhysicsLayer nativeLayer{ layer.GetValue() };

			switch (nativeLayer)
			{
			case Game::PhysicsLayer::NON_MOVING: return "NON_MOVING";
			case Game::PhysicsLayer::MOVING: return "MOVING";
			default: throw Game::Exception("Unknown broad phase layer");
			}
		}
#endif
	};

	class SimpleObjectVsBroadPhaseLayerFilter : public ::JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide(::JPH::ObjectLayer layer1, ::JPH::BroadPhaseLayer layer2) const override
		{
			return std::ranges::any_of(
				std::set{ Game::PhysicsLayer{layer1}, Game::PhysicsLayer{layer2.GetValue()} },
				[](const auto& e) { return e == Game::PhysicsLayer::MOVING; });
		}
	};

	class SimpleObjectLayerPairFilter : public ::JPH::ObjectLayerPairFilter
	{
	public:
		virtual bool ShouldCollide(::JPH::ObjectLayer layer1, ::JPH::ObjectLayer layer2) const override
		{
			return std::ranges::any_of(
				std::set{ Game::PhysicsLayer{layer1}, Game::PhysicsLayer{layer2} },
				[](const auto& e) { return e == Game::PhysicsLayer::MOVING; });
		}
	};

	void JoltTrace(const char* fmt, ...)
	{
		va_list list;
		va_start(list, fmt);
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), fmt, list);
		va_end(list);

		Game::Logger::Info("JOLT TRACE: {}", buffer);
	}

}

namespace Game {

	struct PhysicsSystem::Implementation
	{
		SimpleBroadPhaseLayer BroadPhaseLayer;
		SimpleObjectVsBroadPhaseLayerFilter ObjectVsBroadPhaseLayerFilter;
		SimpleObjectLayerPairFilter ObjectLayerPairFilter;
		::JPH::TempAllocatorImpl TempAllocator = ::JPH::TempAllocatorImpl{ 10u * 1024u * 1024u };
		::JPH::JobSystemThreadPool JobSystem = ::JPH::JobSystemThreadPool(::JPH::cMaxPhysicsJobs, ::JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1u);
		::JPH::PhysicsSystem PhysicsSystem;
		::JPH::BodyID Sphere;
		DebugRenderer Debug_Renderer = { {} };
	};

	PhysicsSystem::PhysicsSystem()
		: m_Impl{}
	{
		static bool once = false;
		if (!once)
		{
			once = true;

			::JPH::RegisterDefaultAllocator();
			::JPH::Trace = JoltTrace;

			::JPH::Factory::sInstance = new ::JPH::Factory{};

			::JPH::RegisterTypes();
		}

		m_Impl = std::make_unique<Implementation>();

		constexpr uint32_t maxBodies = 1024u;
		constexpr uint32_t numBodyMutexes = 0u;
		constexpr uint32_t maxBodyPairs = 1024u;
		constexpr uint32_t maxContactConstraints = 1024u;

		m_Impl->PhysicsSystem.Init(
			maxBodies,
			numBodyMutexes,
			maxBodyPairs,
			maxContactConstraints,
			m_Impl->BroadPhaseLayer,
			m_Impl->ObjectVsBroadPhaseLayerFilter,
			m_Impl->ObjectLayerPairFilter);

		m_Impl->PhysicsSystem.SetGravity({ 0.0f, -9.8f, 0.0f });
	}

	PhysicsSystem::~PhysicsSystem() = default;

	void PhysicsSystem::Update()
	{
		m_Impl->Debug_Renderer.Clear();

		m_Impl->PhysicsSystem.Update(1.0f / 60.0f, 1, &m_Impl->TempAllocator, &m_Impl->JobSystem);

		static const ::JPH::BodyManager::DrawSettings settings{};
		m_Impl->PhysicsSystem.DrawBodies(settings, &m_Impl->Debug_Renderer);
	}

	const DebugRenderer& PhysicsSystem::Debug_Renderer() const
	{
		return m_Impl->Debug_Renderer;
	}

	RigidBody PhysicsSystem::CreateRigidBody(const Shape& shape, const vec3& position, RigidBodyType type) const
	{
		auto& bodyInterface = m_Impl->PhysicsSystem.GetBodyInterface();
		return { shape, position, type, bodyInterface, {} };
	}

}
