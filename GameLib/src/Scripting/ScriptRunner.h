#pragma once

#include "Scripting/LuaScript.h"

namespace Game {

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

		template <class... Args>
		void Execute(const std::string& functionName, Args&&... args) const
		{
			const auto setArgs = [this]<class... Args>(Args&&... args)
			{
				(m_Script.SetArgument(args), ...);
			};

			m_Script.SetFunction(functionName);
			SetArgs(std::forward<Args>(args)...);

			m_Script.Execute(sizeof...(args), 0u);
		}

	private:
		LuaScipt& m_Script;
	};

}
