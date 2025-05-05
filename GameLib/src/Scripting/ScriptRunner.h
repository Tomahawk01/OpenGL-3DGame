#pragma once

#include "Scripting/LuaScript.h"

#include <tuple>

namespace Game {

	namespace impl {

		template <size_t I, size_t Size, class T>
		auto GetResult(LuaScipt& script, T& resultTuple)
		{
			script.GetResult(std::get<Size - I - 1>(resultTuple));

			if constexpr (I != Size - 1)
			{
				GetResult<I + 1, Size>(script, resultTuple);
			}
		}

	}

	class ScriptRunner
	{
	public:
		ScriptRunner(LuaScipt& script)
			: m_Script(script)
		{}

		ScriptRunner(const ScriptRunner&) = delete;
		ScriptRunner(ScriptRunner&&) = delete;
		ScriptRunner& operator=(const ScriptRunner&) = delete;
		ScriptRunner& operator=(ScriptRunner&&) = delete;

		template <class... R, class... Args>
		auto Execute(const std::string& functionName, Args&&... args) const
		{
			const auto setArgs = [this](Args&&... args)
			{
				(m_Script.SetArgument(args), ...);
			};

			m_Script.SetFunction(functionName);
			SetArgs(std::forward<Args>(args)...);

			m_Script.Execute(sizeof...(args), sizeof...(R));

			if constexpr (sizeof...(R) == 0)
			{
				return;
			}
			else if constexpr (sizeof...(R) == 1)
			{
				auto res = std::tuple<R...>{};
				m_Script.GetResult(std::get<0>(res));
				return std::get<0>(res);
			}
			else
			{
				auto res = std::tuple<R...>{};
				impl::GetResult<0, sizeof...(R)>(m_Script, res);
				return res;
			}
		}

	private:
		LuaScipt& m_Script;
	};

}
