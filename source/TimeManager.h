#pragma once
#include <string>

namespace lgtm
{	
	class TimeManager
	{	
	public:
		TimeManager() = default;

		inline std::string getDayStamp() const
		{
            auto now = std::chrono::utc_clock::now();

			return std::format("{:%F}", now);
		}

		inline std::string getTimeStamp() const
        {
            auto now = std::chrono::utc_clock::now();

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            return std::format("{:%F :%H:%M:%S}.{:03}", std::chrono::floor<std::chrono::seconds>(now), ms.count());
		}
	};
}