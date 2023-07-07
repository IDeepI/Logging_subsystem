#pragma once
#include "Singleton.h"

namespace lg
{	
	class LogManager : public Singleton<LogManager>
	{	
	public:
		template<typename... Args>
		static inline void Log(std::string_view path, Args... args)
		{

		}

	};
}