#pragma once

namespace fg
{	
    struct FileGuard
    {
        FileGuard(const std::filesystem::path& fullPath): _fullPath(fullPath)
        {            
            std::shared_lock<std::shared_mutex> cLock(_containerMutex);
            if (_container.contains(_fullPath)) {
                _container.at(_fullPath).lock();
                return;
            }    
            cLock.unlock();

            std::unique_lock<std::shared_mutex> uniqueContainerLock(_containerMutex);
            _container[std::filesystem::path(_fullPath)];
            _container.at(_fullPath).lock();
        }

        ~FileGuard()
        {
            std::shared_lock<std::shared_mutex> cLock(_containerMutex);
            _container.at(_fullPath).unlock();
        }

        static std::shared_mutex _containerMutex;
        static std::map<std::filesystem::path, std::mutex> _container;
        std::filesystem::path _fullPath;
    };

    __declspec(selectany) std::shared_mutex FileGuard::_containerMutex;

    __declspec(selectany) std::map<std::filesystem::path, std::mutex> FileGuard::_container;
}