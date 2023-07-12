#pragma once

using fileStream = std::pair<std::fstream, std::mutex>;
using streamPtr = std::shared_ptr<fileStream>;
using atomicStreamPtr = std::atomic<streamPtr>;

namespace fg
{	
    class FileGuard
    {
    public:
        FileGuard(const std::filesystem::path& fullPath): _fullPath(fullPath)
        {   
            // Check if path is in container
            std::shared_lock<std::shared_mutex> cLock(_containerMutex);
            if (_container.contains(_fullPath)) {
                _streamPtr = _container.at(_fullPath).load();
                return;
            }
            cLock.unlock();

            std::unique_lock<std::shared_mutex> uniqueContainerLock(_containerMutex);
            if (!createDirectory(_fullPath.parent_path()))
                return;

            _container[_fullPath].store(std::make_shared<fileStream>());
            _streamPtr = _container.at(_fullPath).load();
            _streamPtr->first = openStream(_fullPath);
        }

        void writeLine(std::string_view msg) {
            std::scoped_lock lock(_streamPtr->second);
            auto& stream = _streamPtr->first;
            if (stream.bad() || stream.fail() || !stream.is_open())
                assert(false);
            _streamPtr->first << msg << std::endl;
        };

        ~FileGuard()
        {
            // Check if shared ptr have only one ref, del pair from container
            std::shared_lock<std::shared_mutex> cLock(_containerMutex);
            if(_streamPtr.use_count() > 2)
                return;
            cLock.unlock();

            std::unique_lock<std::shared_mutex> uniqueContainerLock(_containerMutex);
            if (_container.contains(_fullPath)) {
                std::erase_if(_container, [_fullPath = _fullPath](const auto& item) {
                    auto const& [key, value] = item;
                    return (key == _fullPath) && (value.load().use_count() < 3);
                    });
            }
        }
    private:
        static std::shared_mutex _containerMutex;
        static std::map<std::filesystem::path, atomicStreamPtr> _container;
        std::filesystem::path _fullPath;
        streamPtr _streamPtr;

        std::fstream openStream(const std::filesystem::path& fullPath){
            std::fstream outfile;
            if (std::filesystem::exists(fullPath))
                outfile.open(fullPath, std::ios_base::app);
            else
                outfile.open(fullPath, std::ios::out | std::ios::in | std::ios_base::app);

            if (!outfile.is_open())
                assert(false);

            return outfile;
        }

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

    __declspec(selectany) std::shared_mutex FileGuard::_containerMutex;

    __declspec(selectany) std::map<std::filesystem::path, atomicStreamPtr> FileGuard::_container;
}