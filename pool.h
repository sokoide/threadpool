#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

// uncomment if you want to use pthread, uncomment
// `#add_definitions(-DUSE_PTHREAD) in CMakeList.txt

class ThreadPool {
  public:
    void Start(int num_threads) {
        for (int i = 0; i < num_threads; i++) {
#ifdef USE_PTHREAD
            pthread_t pt;
            pthread_create(&pt, nullptr, ThreadPool::LoopWrapper, this);
            threads.emplace_back(pt);
#else
            threads.push_back(std::thread(&ThreadPool::Loop, this));
            ;
#endif
        }
    }

    void Stop() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            should_terminate = true;
        }
        cond.notify_all();
#ifdef USE_PTHREAD
        for (auto& active_thread : threads) {
            pthread_join(active_thread, nullptr);
        }
#else
        for (auto& active_thread : threads) {
            active_thread.join();
        }
#endif
        threads.clear();
    }

    std::future<void> Queue(const std::function<void()>& task) {
        std::packaged_task<void()> ptask(task);
        std::future<void> f = ptask.get_future();
        {
            std::lock_guard<std::mutex> lock(mtx);
            // tasks.emplace(std::move(ptask));
            tasks.push(std::move(ptask));
        }

        cond.notify_one();
        return f;
    }

  private:
    void Loop() {
        while (true) {
            std::packaged_task<void()> task;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cond.wait(lock, [this] {
                    return !tasks.empty() || should_terminate;
                });
                if (should_terminate)
                    return;
                if (tasks.empty())
                    continue;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    static void* LoopWrapper(void* p) {
        reinterpret_cast<ThreadPool*>(p)->Loop();
        return nullptr;
    }

    bool should_terminate = false;

    std::mutex mtx;
    std::condition_variable cond;
#ifdef USE_PTHREAD
    std::vector<pthread_t> threads;
#else
    std::vector<std::thread> threads;
#endif
    std::queue<std::packaged_task<void()>> tasks;
};
