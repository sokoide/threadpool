#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

using namespace std;

// uncomment if you want to use pthread
// #define USE_PTHREAD

class ThreadPool {
  public:
    void Start(int num_threads) {
        for (int i = 0; i < num_threads; i++) {
#ifdef USE_PTHREAD
            pthread_t pt;
            pthread_create(&pt, nullptr, ThreadPool::LoopWrapper, this);
            threads.emplace_back(pt);
#else
            threads.emplace_back(thread(&ThreadPool::Loop, this));
            ;
#endif
        }
    }

    void Stop() {
        {
            unique_lock<mutex> lock(mtx);
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
            unique_lock<mutex> lock(mtx);
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
                unique_lock<mutex> lock(mtx);
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

    mutex mtx;
    condition_variable cond;
#ifdef USE_PTHREAD
    vector<pthread_t> threads;
#else
    vector<thread> threads;
#endif
    queue<function<void()>> tasks;
};
