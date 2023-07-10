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

			return std::format("%F", std::chrono::floor<std::chrono::days>(now.time_since_epoch()));
		}

		inline std::string getTimeStamp() const
        {
            auto now = std::chrono::utc_clock::now();

            std::ostringstream oss;
            oss << std::format("%F %T", std::chrono::floor<std::chrono::seconds>(now.time_since_epoch()));
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            oss << std::setw(3) << std::setfill('0') << ms.count();

            return oss.str();
		}

	};
}