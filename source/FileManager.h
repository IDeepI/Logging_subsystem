#pragma once
#include <fstream>

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
            createDirectory(filePath.parent_path());

            std::fstream outfile;
            if(std::filesystem::exists(filePath))
                outfile.open(filePath, std::ios_base::app);
            else
                outfile.open(filePath, std::ios::out | std::ios::in | std::ios_base::app);

            if (!outfile.is_open())
                assert(false);
            outfile << msg << std::endl;
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