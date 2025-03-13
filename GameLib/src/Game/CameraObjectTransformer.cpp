#include "CameraObjectTransformer.h"

namespace Game {

	CameraObjectTransformer::CameraObjectTransformer(const vec3& position, const Camera& camera)
		: m_Position(position)
		, m_Camera(camera)
		, m_CameraLastPosition(camera.GetPosition())
	{}

	void CameraObjectTransformer::Update()
	{
		m_Position += m_Camera.GetPosition() - m_CameraLastPosition;
		m_CameraLastPosition = m_Camera.GetPosition();
	}

	vec3 CameraObjectTransformer::Position() const
	{
		return m_Position;
	}

}
