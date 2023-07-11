#pragma once

using fileStream = std::pair<std::fstream, std::mutex>;

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
                _streamPtr = _container.at(_fullPath);
                return;
            }
            cLock.unlock();

            std::unique_lock<std::shared_mutex> uniqueContainerLock(_containerMutex);
            _streamPtr = std::make_shared<fileStream>();
            _streamPtr->first = openStream(_fullPath);
            _container[std::filesystem::path(_fullPath)] = _streamPtr;
        }

        void writeLine(std::string_view msg) {
            std::scoped_lock lock(_streamPtr->second);
            _streamPtr->first << msg << std::endl;
        };

        ~FileGuard()
        {
            // Check if shared ptr have only one ref del pair from container
            std::unique_lock<std::shared_mutex> uniqueContainerLock(_containerMutex);

            //TEST:
//            std::cout << _streamPtr.use_count() << " " << _fullPath << std::endl;

            if(_streamPtr.use_count() > 1)
                return;

            _container.erase(_fullPath);
        }
    private:
        static std::shared_mutex _containerMutex;
        static std::map<std::filesystem::path, std::shared_ptr<fileStream>> _container;
        std::filesystem::path _fullPath;
        std::shared_ptr<fileStream> _streamPtr;

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
    };

    __declspec(selectany) std::shared_mutex FileGuard::_containerMutex;

    __declspec(selectany) std::map<std::filesystem::path, std::shared_ptr<fileStream>> FileGuard::_container;
}