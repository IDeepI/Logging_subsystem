#pragma once

class ThreadPool {
public:

    ThreadPool() : _stop(false)
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

    template<typename Func, typename... Args>
    decltype(auto) enqueue(Func&& func, Args&&... args) {
        using ReturnType = typename std::invoke_result_t<Func, Args...>;
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::future<ReturnType> result = task->get_future();
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.emplace([task]() { (*task)(); });
        _condition.notify_one();
        return result;
    }

    bool busy() { return !_tasks.empty(); };

    ~ThreadPool()
    {
        _stop = true;
        _condition.notify_all();
        for (auto& fut : _futures) {
            fut.get();
        }
    }

private:
    std::vector<std::future<void>> _futures;
    std::queue<std::function<void()>> _tasks;
    std::condition_variable _condition;
    std::mutex _mutex;
    std::atomic<bool> _stop;
};

using ThreadPoolPtr = std::unique_ptr<ThreadPool>;