#pragma once
#include <string>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace jam::scripting
{
	struct Script
	{
		sol::state state;
		sol::table self;

		bool enabled = false;
		void Initialize();
		bool LoadFile(std::string filepath);
		template<typename... Args>
		inline bool ExecuteAny(std::string functionName, Args&&... args);

		template<typename... Args>
		inline bool ExecuteSelf(std::string functionName, Args&&... args);

		template<typename... Args>
		inline bool ExecuteFunction(sol::function, Args&&... args);

		bool ExecuteCommand(std::string command, char* outBuf = nullptr);
	};

	void catch_sol_error(const sol::error& error);
}

namespace jam::scripting
{
	template<typename ...Args>
	inline bool Script::ExecuteAny(std::string functionName, Args && ...args)
	{
		sol::protected_function func = state[functionName];
		if (func)
		{
			auto result = func(std::forward<Args>(args)...);
			bool isValid = result.valid();
			if (!isValid)
			{
				catch_sol_error(result);
			}
			return isValid;
		}
		return false;
	}

	template<typename ...Args>
	inline bool Script::ExecuteSelf(std::string functionName, Args && ...args)
	{
		if (!self.valid())
			return false;

		sol::protected_function func = self[functionName];
		if (func)
		{
			auto result = func(self, std::forward<Args>(args)...);
			bool isValid = result.valid();
			if (!isValid)
			{
				catch_sol_error(result);
			}
			return isValid;
		}
		return false;
	}

	template<typename ...Args>
	inline bool Script::ExecuteFunction(sol::function input, Args && ...args)
	{
		sol::protected_function func = input;
		if (func)
		{
			auto result = func(std::forward<Args>(args)...);
			bool isValid = result.valid();
			if (!isValid)
			{
				catch_sol_error(result);
			}
			return isValid;
		}
		return false;
	}
}