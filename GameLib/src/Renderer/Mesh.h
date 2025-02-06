#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"
#include "Buffer.h"
#include "MeshLoader.h"
#include "MeshData.h"

namespace Game {

	class TLVReader;

	class Mesh
	{
	public:
		Mesh(const MeshData& data);
		Mesh(const TLVReader& reader, std::string_view name);

		void Bind() const;
		void UnBind() const;

		uint32_t IndexCount() const;
		uintptr_t IndexOffset() const;

	private:
		AutoRelease<::GLuint> m_VAO;
		Buffer m_VBO;

		uint32_t m_IndexCount;
		uintptr_t m_IndexOffset;
	};

}
