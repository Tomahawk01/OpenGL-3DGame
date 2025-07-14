#pragma once

#include <coroutine>
#include <ranges>
#include <stdexcept>

namespace Game {

	class Task
	{
	public:
		struct promise_type
		{
			auto initial_suspend()
			{
				return std::suspend_always{};
			}

			auto final_suspend() noexcept
			{
				return std::suspend_always{};
			}

			auto return_void()
			{}

			auto get_return_object()
			{
				return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			auto unhandled_exception()
			{
				ExceptionPtr = std::current_exception();
			}

			std::exception_ptr ExceptionPtr = nullptr;
		};
		
		~Task()
		{
			if (m_Handle)
			{
				m_Handle.destroy();
			}
		}

		Task(const Task&) = delete;
		Task& operator=(const Task&) = delete;

		Task(Task&& other)
			: m_Handle(std::exchange(other.m_Handle, nullptr))
		{}

		Task& operator=(Task&& other)
		{
			std::ranges::swap(m_Handle, other.m_Handle);
			return *this;
		}

		bool CanResume() const
		{
			return m_Handle && !m_Handle.done();
		}

		bool IsThisTask(std::coroutine_handle<> handle) const
		{
			return m_Handle == handle;
		}

		void Resume()
		{
			m_Handle.resume();

			if (m_Handle && m_Handle.promise().ExceptionPtr)
			{
				std::rethrow_exception(m_Handle.promise().ExceptionPtr);
			}
		}

		std::coroutine_handle<> NativeHandle() const
		{
			return m_Handle;
		}

	private:
		explicit Task(std::coroutine_handle<promise_type> handle)
			: m_Handle(handle)
		{}

	private:
		std::coroutine_handle<promise_type> m_Handle;
	};

}
