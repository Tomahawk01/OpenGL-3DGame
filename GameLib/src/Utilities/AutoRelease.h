#pragma once

#include <functional>
#include <ranges>

namespace Game {

	template<class T, T Invalid = {}>
	class AutoRelease
	{
	public:
		AutoRelease()
			: AutoRelease(Invalid, nullptr)
		{}

		AutoRelease(T obj, std::function<void(T)> deleter)
			: m_Object(obj), m_Deleter(deleter)
		{}

		~AutoRelease()
		{
			if ((m_Object != Invalid) && m_Deleter)
				m_Deleter(m_Object);
		}

		AutoRelease(const AutoRelease&) = delete;
		AutoRelease& operator=(const AutoRelease&) = delete;

		AutoRelease(AutoRelease&& other)
			: AutoRelease()
		{
			Swap(other);
		}

		AutoRelease& operator=(AutoRelease&& other)
		{
			AutoRelease new_obj{ std::move(other) };
			Swap(new_obj);

			return *this;
		}

		void Swap(AutoRelease& other) noexcept
		{
			std::ranges::swap(m_Object, other.m_Object);
			std::ranges::swap(m_Deleter, other.m_Deleter);
		}

		T Get() const { return m_Object; }
		operator T() const { return m_Object; }
		explicit operator bool() const { return m_Object != Invalid; }
		T* operator&() noexcept { return std::addressof(m_Object); }

	private:
		T m_Object;
		std::function<void(T)> m_Deleter;
	};

}
