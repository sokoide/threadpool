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

    template <typename F, typename... Args>
    inline auto Queue(F&& f,
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
            std::lock_guard<std::mutex> lock(mtx);
            tasks.emplace(wrapper_func);
        }

        cond.notify_one();
        return task_ptr->get_future();
    }

  private:
    void Loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cond.wait(lock, [this] {
                    return !tasks.empty() || should_terminate;
                });
                if (should_terminate) {
                    return;
                }
                if (tasks.empty())
                    continue;
                task = tasks.front();
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
    std::queue<std::function<void()>> tasks;
};
