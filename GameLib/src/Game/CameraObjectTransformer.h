#pragma once

#include "ObjectTransformer.h"
#include "Renderer/Camera.h"

namespace Game {

	class CameraObjectTransformer : public ObjectTransformer
	{
	public:
		CameraObjectTransformer(const vec3& position, const Camera& camera);
		~CameraObjectTransformer() override = default;

		void Update() override;
		vec3 Position() const override;

	private:
		vec3 m_Position;
		const Camera& m_Camera;
		vec3 m_CameraLastPosition;
	};

}
