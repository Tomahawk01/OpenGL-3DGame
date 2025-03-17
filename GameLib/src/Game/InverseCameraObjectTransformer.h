#pragma once

#include "ObjectTransformer.h"
#include "Renderer/Camera.h"

namespace Game {

	class InverseCameraObjectTransformer : public ObjectTransformer
	{
	public:
		InverseCameraObjectTransformer(const vec3& position, const Camera& camera);
		~InverseCameraObjectTransformer() override = default;

		void Update() override;
		vec3 Position() const override;

	private:
		vec3 m_Position;
		const Camera& m_Camera;
		vec3 m_CameraLastPosition;
	};

}
