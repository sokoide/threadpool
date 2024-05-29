#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

// uncomment if you want to use pthread, uncomment
// `#add_definitions(-DUSE_PTHREAD) in CMakeList.txt

template <typename T> class ThreadPoolSpecific {
  public:
    void Start(int num_threads) {
        for (int i = 0; i < num_threads; i++) {
#ifdef USE_PTHREAD
            pthread_t pt;
            pthread_create(&pt, nullptr, ThreadPoolSpecific::LoopWrapper, this);
            _threads.emplace_back(pt);
#else
            _threads.push_back(std::thread(&ThreadPoolSpecific::Loop, this));
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

    std::future<T> Queue(const std::function<T()>& task) {
        std::packaged_task<T()> ptask(task);
        std::future<T> fut = ptask.get_future();

        {
            std::lock_guard<std::mutex> lock(_mtx);
            _tasks.emplace(std::move(ptask));
        }

        _cond.notify_one();
        return fut;
    }

  private:
    void Loop() {
        while (true) {
            std::packaged_task<T()> task;
            {
                std::unique_lock<std::mutex> lock(_mtx);
                _cond.wait(lock, [this] {
                    return !_tasks.empty() || _should_terminate;
                });
                if (_should_terminate) {
                    return;
                }
                if (_tasks.empty())
                    continue;
                task = std::move(_tasks.front());
                _tasks.pop();
            }
            task();
        }
    }

    static void* LoopWrapper(void* p) {
        reinterpret_cast<ThreadPoolSpecific*>(p)->Loop();
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
    std::queue<std::packaged_task<T()>> _tasks;
};
