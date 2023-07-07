#pragma once

class ThreadPool {
public:
    ThreadPool();

    template<typename Func, typename... Args>
    auto enqueue(Func&& func, Args&&... args) -> std::future<typename std::invoke_result_t<Func, Args...>> {
        using ReturnType = typename std::invoke_result_t<Func, Args...>;
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::future<ReturnType> result = task->get_future();
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.emplace([task]() { (*task)(); });
        _condition.notify_one();
        return result;
    }

    ~ThreadPool();

private:
    std::vector<std::future<void>> _futures;
    std::queue<std::function<void()>> _tasks;
    std::condition_variable _condition;
    std::mutex _mutex;
    bool _stop;
};

using ThreadPoolPtr = std::unique_ptr<ThreadPool>;