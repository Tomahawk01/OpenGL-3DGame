#include "InverseCameraObjectTransformer.h"

namespace Game {

	InverseCameraObjectTransformer::InverseCameraObjectTransformer(const vec3& position, const Camera& camera)
		: m_Position(position)
		, m_Camera(camera)
		, m_CameraLastPosition(camera.GetPosition())
	{
	}

	void InverseCameraObjectTransformer::Update()
	{
		m_Position += m_CameraLastPosition - m_Camera.GetPosition();
		m_CameraLastPosition = m_Camera.GetPosition();
	}

	vec3 InverseCameraObjectTransformer::Position() const
	{
		return m_Position;
	}

}
