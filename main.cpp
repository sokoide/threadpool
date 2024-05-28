#include "headers.hpp"
#include "pool.h"

using namespace std;
// using namespace atcoder;

using ll = long long;
using ll = long long;
using vll = vector<ll>;
using vvll = vector<vll>;
using vvvll = vector<vvll>;
using pll = pair<ll, ll>;

ThreadPool pool;

// void init_thread_pool(size_t num_threads) {
//     for (size_t i = 0; i < num_threads; ++i) {
//         threads.emplace_back(std::thread([]() {
//             while (true) {
//                 // Get a task from the task queue
//                 // ...
//             }
//         }));
//     }
// }
//
// template <typename F, typename... Args>
// auto push_task(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
//     std::packaged_task<decltype(f(args...))()> task(
//         std::bind(std::forward<F>(f), std::forward<Args>(args)...));
//     auto future = task.get_future();
//     // Push the task to the task queue
//     // ...
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         jobs.push(task, args...);
//     }
//     mutex_condition.notify_one();
//     return future;
// }
//
// int compute_result(int x, int y) {
//     std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate some
//     work return x + y;
// }

int main() {
    pool.Start(3);
    vector<future<int>> v;

    for (int i = 0; i < 5; i++) {
        future<int> f = pool.Queue([i]() -> int {
            using namespace chrono_literals;
            this_thread::sleep_for(1s);
            auto now = chrono::system_clock::now();
            auto nowt = chrono::system_clock::to_time_t(now);
            printf("* %d %s", i, ctime(&nowt));
            return i * 10;
        });
        v.emplace_back(std::move(f));
    }

    for (auto& f : v) {
        printf("result: %d\n", f.get());
    }

    pool.Stop();
    return 0;
}
