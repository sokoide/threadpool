#include "headers.hpp"
#include "pool.h"

using ll = long long;
using ll = long long;
using vll = std::vector<ll>;
using vvll = std::vector<vll>;
using vvvll = std::vector<vvll>;
using pll = std::pair<ll, ll>;

ThreadPool pool;

int main() {
    pool.Start(3);
    std::vector<std::future<void>> v;
    // std::vector<std::packaged_task<void()>> v;

    for (int i = 0; i < 5; i++) {
        std::future<void> f = pool.Queue([i] {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
            auto now = std::chrono::system_clock::now();
            auto nowt = std::chrono::system_clock::to_time_t(now);
            printf("* %d %s", i, ctime(&nowt));
            return i * 10;
        });
        v.emplace_back(std::move(f));

        // func with bounded parameters to make it ready to execute

        // std::packaged_task<void()> task([i]() {
        //     using namespace std::chrono_literals;
        //     std::this_thread::sleep_for(1s);
        //     auto now = std::chrono::system_clock::now();
        //     auto nowt = std::chrono::system_clock::to_time_t(now);
        //     printf("* %d %s", i, ctime(&nowt));
        // });
        // auto f = pool.Queue(std::move(task));
        // v.emplace_back(std::move(f));
    }

    for (auto& f : v) {
        f.get();
        // printf("result: %d\n", f.get());
    }

    pool.Stop();
    return 0;
}
