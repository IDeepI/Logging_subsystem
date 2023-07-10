#pragma once
#include "Singleton.h"
#include "ThreadPool.h"
#include "TimeManager.h"
#include "FileManager.h"
#define WAIT_ALL_TASKS_MS 100

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
        ~LogManager() {
            while (_pool.busy())
                Sleep(WAIT_ALL_TASKS_MS);
        };

		template<typename... Args>
		inline void log(std::string pathStr, Args... args)
		{
            try
            {	
				_pool.enqueue(&LogManager::logImpl<Args...>, this, std::move(pathStr), std::make_tuple(args...));
            }
            catch (...)
            {
            }
		}

        std::filesystem::path getRoot() const {
            std::scoped_lock(_rootMutex);
            return _root;
        }

        void setRoot(std::filesystem::path val) {
            std::scoped_lock(_rootMutex); 
            _root = val.make_preferred();
        }

    private:
        ThreadPool _pool{};
        std::mutex _rootMutex;
		std::filesystem::path _root{std::filesystem::current_path()};

        template<typename... Ts>
        void logImpl(std::string pathStr, std::tuple<Ts...> tuple)
        {
            try
            {
                lgtm::TimeManager time{};
                std::stringstream msg;
                msg << time.getTimeStamp();
                msg << ", ";
                msg << std::this_thread::get_id();
                msg << ": ";
                msg << tuple;

                fm::FileManager fileSys{};
                auto fullPath = fileSys.generateFilePath(pathStr, getRoot());

                fileSys.writeLog(fullPath, msg.str());
            }
            catch (...)
            {
            }
        }
	};
}

LogManager& getLogManager() {
    return LogManager::instance();
}