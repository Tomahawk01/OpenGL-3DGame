#include "Material.h"

#include "Logger.h"
#include "OpenGL.h"

namespace Game {

	Material::Material(const Shader& vertexShader, const Shader& fragmentShader)
		: m_Handle{}
		, m_Uniforms{}
	{
		Ensure(vertexShader.GetType() == ShaderType::VERTEX, "Shader is not a vertex shader");
		Ensure(fragmentShader.GetType() == ShaderType::FRAGMENT, "Shader is not a fragment shader");

		m_Handle = AutoRelease<::GLuint>{
			::glCreateProgram(),
			::glDeleteProgram
		};
		Ensure(m_Handle, "Failed to create OpenGL program");

		::glAttachShader(m_Handle, vertexShader.GetNativeHandle());
		::glAttachShader(m_Handle, fragmentShader.GetNativeHandle());
		::glLinkProgram(m_Handle);

		::GLint result{};
		::glGetProgramiv(m_Handle, GL_LINK_STATUS, &result);

		if (result != GL_TRUE)
		{
			char log[512];
			::glGetProgramInfoLog(m_Handle, sizeof(log), nullptr, log);

			Game::Ensure(result, "Failed to link program\n{}", log);
		}

		::GLint uniformCount{};
		::glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &uniformCount);

		if (uniformCount != 0)
		{
			::GLint maxNameLength{};
			::glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
			Logger::Info("Max length: {}", maxNameLength);

			::GLsizei length{};
			::GLsizei count{};
			::GLenum type{};

			for (auto i = 0; i < uniformCount; i++)
			{
				std::string name(maxNameLength, '\0');
				::glGetActiveUniform(m_Handle, i, maxNameLength, &length, &count, &type, name.data());
				name.resize(length);

				const ::GLint location = ::glGetUniformLocation(m_Handle, name.c_str());
				m_Uniforms[name] = location;

				Logger::Trace("Fount uniform: {}", name);
			}
		}

		Logger::Info("New material ({} uniforms)", uniformCount);
	}

	void Material::Use() const
	{
		::glUseProgram(m_Handle);
	}

	void Material::SetUniform(std::string_view name, const mat4& obj) const
	{
		const auto uniform = m_Uniforms.find(name);
		Ensure(uniform != std::ranges::cend(m_Uniforms), "Missing uniform {}", name);

		::glUniformMatrix4fv(uniform->second, 1, GL_FALSE, obj.data().data());
	}

	void Material::SetUniform(std::string_view name, int obj) const
	{
		const auto uniform = m_Uniforms.find(name);
		Ensure(uniform != std::ranges::cend(m_Uniforms), "Missing uniform {}", name);

		::glUniform1i(uniform->second, obj);
	}

	void Material::BindTexture(std::uint32_t index, const Texture* texture, const Sampler* sampler) const
	{
		::glBindTextureUnit(index, texture->GetNativeHandle());
		::glBindSampler(index, sampler->GetNativeHandle());

		const std::string uniformName = std::format("tex{}", index);

		SetUniform(uniformName, index);
	}

	void Material::BindTextures(std::span<const std::tuple<const Texture*, const Sampler*>> texSamps) const
	{
		for (const auto& [index, texSamp] : texSamps | std::views::enumerate)
		{
			const auto& [texture, sampler] = texSamp;
			BindTexture(static_cast<std::uint32_t>(index), texture, sampler);
		}
	}

	::GLuint Material::GetNativeHandle() const
	{
		return m_Handle;
	}

}
