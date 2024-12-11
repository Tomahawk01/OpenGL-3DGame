#pragma once

#include "Utilities/AutoRelease.h"
#include "Utilities/StringMap.h"
#include "Math/Matrix4.h"
#include "Shader.h"
#include "Texture.h"
#include "Sampler.h"

namespace Game {

	class Material
	{
	public:
		Material(const Shader& vertexShader, const Shader& fragmentShader);

		void Use() const;
		void SetUniform(std::string_view name, const mat4& obj) const;
		void SetUniform(std::string_view name, int obj) const;
		void BindTexture(std::uint32_t index, const Texture* texture, const Sampler* sampler) const;
		void BindTextures(std::span<const std::tuple<const Texture*, const Sampler*>> texSamps) const;

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		StringMap<::GLuint> m_Uniforms;
	};

}
