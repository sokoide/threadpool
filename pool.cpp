#include "pool.h"

void ThreadPool::Start(int num_threads) {
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

void ThreadPool::Stop() {
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

future<int> ThreadPool::Queue(const function<int()>& task) {
    packaged_task<int()> ptask(task);
    future<int> f = ptask.get_future();
    {
        unique_lock<mutex> lock(mtx);
        tasks.emplace(std::move(ptask));
    }
    cond.notify_one();
    return f;
}

void ThreadPool::Loop() {
    while (true) {
        packaged_task<int()> task;
        {
            unique_lock<mutex> lock(mtx);
            cond.wait(lock,
                      [this] { return !tasks.empty() || should_terminate; });
            if (should_terminate) {
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

void* ThreadPool::LoopWrapper(void* p) {
    reinterpret_cast<ThreadPool*>(p)->Loop();
    return nullptr;
}

// to use it
// thread_pool->QueueJob([] { /* ... */ });
