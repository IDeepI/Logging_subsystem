#include "StdAfx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool() : _stop(false)
{
    const auto threadCount = std::thread::hardware_concurrency();
    for (size_t i = 0; i < threadCount; ++i) {
        auto processTask = [this] {
            while (true) {
                std::unique_lock<std::mutex> lock(_mutex);
                _condition.wait(lock, [this] {
                    return !_tasks.empty() || _stop;
                    });
                if (_stop && _tasks.empty()) {
                    return;
                }
                auto task = std::move(_tasks.front());
                _tasks.pop();
                lock.unlock();
                task();
            }
        };
        _futures.emplace_back(std::async(std::launch::async, processTask));
    }
}

ThreadPool::~ThreadPool()
{
    _stop = true;
    _condition.notify_all();
    for (auto& fut : _futures) {
        fut.get();
    }
}
