#pragma once

#include "Utilities/AutoRelease.h"
#include "Utilities/StringMap.h"
#include "Math/Matrix4.h"
#include "Math/Color.h"
#include "Shader.h"
#include "Texture.h"
#include "Sampler.h"
#include "CubeMap.h"

#include <functional>

namespace Game {

	class Entity;

	class Material
	{
	public:
		using UniformCallback = std::function<void(const Material*, const Entity*)>;

		Material(const Shader& vertexShader, const Shader& fragmentShader);

		void Use() const;
		void SetUniform(std::string_view name, const mat4& obj) const;
		void SetUniform(std::string_view name, const Color& obj) const;
		void SetUniform(std::string_view name, int obj) const;
		void SetUniform(std::string_view name, float obj) const;
		void BindCubeMap(const CubeMap* cubeMap, const Sampler* sampler) const;
		void BindTexture(uint32_t index, const Texture* texture, const Sampler* sampler) const;
		void BindTexture(uint32_t index, const Texture* texture) const;
		void BindTextures(std::span<const Texture* const> texSamps) const;

		void SetUniformCallback(UniformCallback uniformCallback);
		void InvokeUniformCallback(const Entity* entity) const;

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		StringMap<::GLuint> m_Uniforms;
		UniformCallback m_UniformCallback;
	};

}
