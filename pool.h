#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

using namespace std;

// #define USE_PTHREAD

class ThreadPool {
  public:
    void Start(int num_threads);
    void Stop();
    future<int> Queue(const function<int()>& task);

  private:
    void Loop();
    static void* LoopWrapper(void* p);
    bool should_terminate = false;

    mutex mtx;
    condition_variable cond;
#ifdef USE_PTHREAD
    vector<pthread_t> threads;
#else
    vector<thread> threads;
#endif
    queue<packaged_task<int()>> tasks;
};
