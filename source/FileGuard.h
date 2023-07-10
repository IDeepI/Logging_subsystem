#pragma once

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
            }
            cLock.unlock();

            _streamPtr = std::make_shared<std::fstream>(std::move(openStream()));
            std::unique_lock<std::shared_mutex> uniqueContainerLock(_containerMutex);
            _container[std::filesystem::path(_fullPath)] = _streamPtr;
        }

        void writeLine(std::string_view msg) {
            (*_streamPtr) << msg << std::endl;
        };

        ~FileGuard()
        {
            // Check if shared ptr have only one ref del pair from container
            std::shared_lock<std::shared_mutex> cLock(_containerMutex);
            if(_container.at(_fullPath).use_count() > 2)
                return;

            cLock.unlock();

            std::unique_lock<std::shared_mutex> uniqueContainerLock(_containerMutex);
            _container.erase(_fullPath);
        }

    private:
        static std::shared_mutex _containerMutex;
        static std::map<std::filesystem::path, std::shared_ptr<std::fstream>> _container;
        std::filesystem::path _fullPath;
        std::shared_ptr<std::fstream> _streamPtr;

        std::fstream openStream(){
            std::fstream outfile;
            if (std::filesystem::exists(_fullPath))
                outfile.open(_fullPath, std::ios_base::app);
            else
                outfile.open(_fullPath, std::ios::out | std::ios::in | std::ios_base::app);

            if (!outfile.is_open())
                assert(false);

            return outfile;
        }
    };

    __declspec(selectany) std::shared_mutex FileGuard::_containerMutex;

    __declspec(selectany) std::map<std::filesystem::path, std::shared_ptr<std::fstream>> FileGuard::_container;
}