#include "headers.hpp"
#include "pool.h"

using namespace std;

using ll = long long;
using ll = long long;
using vll = vector<ll>;
using vvll = vector<vll>;
using vvvll = vector<vvll>;
using pll = pair<ll, ll>;

ThreadPool pool;

int main() {
    pool.Start(3);
    vector<future<int>> v;

    for (int i = 0; i < 5; i++) {
        auto func = ([i] {
            using namespace chrono_literals;
            this_thread::sleep_for(1s);
            auto now = chrono::system_clock::now();
            auto nowt = chrono::system_clock::to_time_t(now);
            printf("* %d %s", i, ctime(&nowt));
            return i * 10;
        });

        auto f = pool.Queue(func);
        v.emplace_back(std::move(f));
    }

    for (auto& f : v) {
        printf("result: %d\n", f.get());
    }

    pool.Stop();
    return 0;
}
