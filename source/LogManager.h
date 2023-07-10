#pragma once
#include "Singleton.h"
#include "ThreadPool.h"
#include "TimeManager.h"
#include "FileManager.h"

namespace
{	
    //Expanding tuple to stream
    template<typename... Ts>
    std::ostream& operator<<(std::ostream& os, std::tuple<Ts...> const& theTuple)
    {
        std::apply
        (
            [&os](Ts const&... tupleArgs)
            {
                std::size_t n{0};
                ((os << tupleArgs << (++n != sizeof...(Ts) ? " " : "")), ...);
            }, theTuple
        );
        return os;
    }

	class LogManager : public Singleton<LogManager>
    {
        friend class ThreadPool;
	public:
		template<typename... Args>
		inline void log(std::string pathStr, Args... args)
		{
			_pool.enqueue(&LogManager::logImpl<Args...>, this, std::move(pathStr), std::make_tuple(args...));
		}

        std::filesystem::path getRoot() const {
            std::scoped_lock(_rootMutex);
            return _root;
        }

        void setRoot(std::filesystem::path val) {
            std::scoped_lock(_rootMutex); 
            _root = val;
        }

    private:
        std::mutex _rootMutex;
		std::filesystem::path _root{std::filesystem::current_path()};
        ThreadPool _pool{};

        template<typename... Ts>
        void logImpl(std::string pathStr, std::tuple<Ts...> tuple)
        {
            lgtm::TimeManager time{};
            std::stringstream msg;
            msg << time.getTimeStamp();
            msg << ",";
            msg << std::this_thread::get_id();
            msg << ":";
            msg << tuple;

            fm::FileManager fileSys{};
            auto fullPath = fileSys.generateFilePath(pathStr, getRoot());

            // should lock then writing to the same file from few threads. How to check it? Mb on creation should check if the path to file already exist and in process?
            fileSys.writeLog(fullPath, msg.str());
        }
	};
}

LogManager& getLogManager() {
    return LogManager::instance();
}