#pragma once
#include "FileGuard.h"

#define DATE_SEPARATOR "_"

namespace fm
{	
	class FileManager
    {
    public:
        FileManager() = default;

        std::filesystem::path generateFilePath(std::string_view pathStr, std::filesystem::path&& root) const
        {
            std::filesystem::path path{pathStr};
            path = path.make_preferred();
            lgtm::TimeManager time{};

            auto fileName = time.getDayStamp() +
                DATE_SEPARATOR +
                path.filename().string() +
                ".log";

            std::filesystem::path fullPath { root };
            fullPath += path.parent_path();
            fullPath /= fileName;

            return fullPath;
        }

        void writeLog(const std::filesystem::path& filePath, std::string_view msg) const
        {
            if (!createDirectory(filePath.parent_path()))
                return;

            fg::FileGuard fg{filePath};
            fg.writeLine(msg);
        }

    private:
        bool createDirectory(const std::filesystem::path& filePath) const
        {
			if (std::filesystem::exists(filePath))
				return true;
			
            std::error_code err;
            if (!std::filesystem::create_directories(filePath, err))
				return false;
			
			return true;
        }
	};
}