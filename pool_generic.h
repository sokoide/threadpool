#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

// uncomment if you want to use pthread, uncomment
// `#add_definitions(-DUSE_PTHREAD` in CMakeList.txt

class ThreadPoolGeneric {
  public:
    void Start(int num_threads) {
        if (num_threads <= 0) {
            throw std::invalid_argument("num_threads must be greater than 0");
        }
        for (int i = 0; i < num_threads; i++) {
#ifdef USE_PTHREAD
            pthread_t pt;
            pthread_create(&pt, nullptr, ThreadPoolGeneric::LoopWrapper, this);
            _threads.emplace_back(pt);
#else
            _threads.push_back(std::thread(&ThreadPoolGeneric::Loop, this));
            ;
#endif
        }
    }

    void Stop() {
        {
            std::lock_guard<std::mutex> lock(_mtx);
            _should_terminate = true;
        }
        _cond.notify_all();
#ifdef USE_PTHREAD
        for (auto& active_thread : _threads) {
            pthread_join(active_thread, nullptr);
        }
#else
        for (auto& active_thread : _threads) {
            active_thread.join();
        }
#endif
        _threads.clear();
    }

    // if you don't need an argument, or args are already bounded,
    // use this simpler version
    // e.g. Queue(func_ptr);
    template <typename F> auto Queue(F&& f) -> std::future<decltype(f())> {
        // shared ptr to be able to copy construct / assign
        auto task_ptr =
            std::make_shared<std::packaged_task<decltype(f())()>>(f);

        // wrap the packaged_task
        std::function<void()> wrapper_func = [task_ptr]() { (*task_ptr)(); };
        {
            std::lock_guard<std::mutex> lock(_mtx);
            _tasks.emplace(wrapper_func);
        }

        _cond.notify_one();
        return task_ptr->get_future();
    }

    // if you want to queue a function with arguments, use this.
    // e.g. QueuTaskWithArgs(func_ptr, 1, 2, 3);
    template <typename F, typename... Args>
    auto QueueWithArgs(F&& f,
                       Args&&... args) -> std::future<decltype(f(args...))> {
        // func with bounded parameters to make it ready to execute
        std::function<decltype(f(args...))()> func =
            std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // shared ptr to be able to copy construct / assign
        auto task_ptr =
            std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // wrap the packaged_task
        std::function<void()> wrapper_func = [task_ptr]() { (*task_ptr)(); };
        {
            std::lock_guard<std::mutex> lock(_mtx);
            _tasks.emplace(wrapper_func);
        }

        _cond.notify_one();
        return task_ptr->get_future();
    }

  private:
    void Loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(_mtx);
                _cond.wait(lock, [this] {
                    return !_tasks.empty() || _should_terminate;
                });
                if (_should_terminate)
                    return;
                if (_tasks.empty())
                    continue;
                task = std::move(_tasks.front());
                _tasks.pop();
            }
            task();
        }
    }

    static void* LoopWrapper(void* p) {
        reinterpret_cast<ThreadPoolGeneric*>(p)->Loop();
        return nullptr;
    }

    bool _should_terminate = false;

    std::mutex _mtx;
    std::condition_variable _cond;
#ifdef USE_PTHREAD
    std::vector<pthread_t> _threads;
#else
    std::vector<std::thread> _threads;
#endif
    std::queue<std::function<void()>> _tasks;
};
