#pragma once

#include "Utilities/AutoRelease.h"
#include "Renderer/LineData.h"
#include "Renderer/Buffer.h"

#include <span>

namespace Game {

	class DebugLines
	{
	public:
		DebugLines(std::span<const LineData> lines);

		void Bind() const;
		void UnBind() const;

		uint32_t Count() const;

	private:
		AutoRelease<::GLuint> m_VAO;
		Buffer m_VBO;
		uint32_t m_Count;
	};

}
