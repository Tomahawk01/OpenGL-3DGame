#include "PhysicsSystem.h"

#include "Utilities/Logger.h"
#include "Utilities/Error.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/Memory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <cstdarg>
#include <thread>

using namespace JPH::literals;

namespace {

	class SimpleBroadPhaseLayer : public ::JPH::BroadPhaseLayerInterface
	{
	public:
		virtual ::JPH::uint GetNumBroadPhaseLayers() const override { return 2; }
		virtual ::JPH::BroadPhaseLayer GetBroadPhaseLayer(::JPH::ObjectLayer layer) const override { return ::JPH::BroadPhaseLayer(static_cast<::JPH::BroadPhaseLayer::Type>(layer)); }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(::JPH::BroadPhaseLayer layer) const override
		{
			switch (layer.GetValue())
			{
			case 0: return "NON_MOVING";
			case 1: return "MOVING";
			default: throw Game::Exception("Unknown broad phase layer");
			}
		}
#endif
	};

	class SimpleObjectVsBroadPhaseLayerFilter : public ::JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide(::JPH::ObjectLayer layer1, ::JPH::BroadPhaseLayer layer2) const override { return layer1 == 1 || layer2.GetValue() == 1; }
	};

	class SimpleObjectLayerPairFilter : public ::JPH::ObjectLayerPairFilter
	{
	public:
		virtual bool ShouldCollide(::JPH::ObjectLayer layer1, ::JPH::ObjectLayer layer2) const override { return layer1 == 1 || layer2 == 1; }
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
		DebugRenderer Debug_Renderer;
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

		auto& bodyInterface = m_Impl->PhysicsSystem.GetBodyInterface();

		auto floorShapeSetting = ::JPH::BoxShapeSettings{ ::JPH::Vec3{100.0f, 1.0f, 100.0f} };
		floorShapeSetting.SetEmbedded();
		auto floorShapeResult = floorShapeSetting.Create();
		Ensure(floorShapeResult.IsValid(), "Invalid shape");
		auto floorShape = floorShapeResult.Get();
		auto floorSettings = ::JPH::BodyCreationSettings{ floorShape, ::JPH::RVec3(0.0_r, -1.0_r, 0.0_r), ::JPH::Quat::sIdentity(), ::JPH::EMotionType::Static, 1 };
		auto* floor = bodyInterface.CreateBody(floorSettings);

		bodyInterface.AddBody(floor->GetID(), ::JPH::EActivation::DontActivate);

		auto sphereSettings = ::JPH::BodyCreationSettings(new ::JPH::SphereShape(5.0f), ::JPH::RVec3(0.0_r, 100.0_r, -10.0_r), ::JPH::Quat::sIdentity(), ::JPH::EMotionType::Dynamic, 0);
		m_Impl->Sphere = bodyInterface.CreateAndAddBody(sphereSettings, ::JPH::EActivation::Activate);

		bodyInterface.SetLinearVelocity(m_Impl->Sphere, ::JPH::Vec3(0.0f, -5.0f, 0.0f));

		m_Impl->PhysicsSystem.OptimizeBroadPhase();
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

}
