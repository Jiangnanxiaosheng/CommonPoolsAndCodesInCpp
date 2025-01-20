#include <iostream>
#include <chrono>
#include <memory>
#include "ThreadPool.hpp"
#include "thread_pool2.h"
#if 1
int main() {

    ThreadPool pool(4);

    // 向线程池中添加任务
    pool.enqueue([] {
        std::this_thread::sleep_for(std::chrono::seconds(4));
        std::cout << "Task 1 completed\n";
    });

    pool.enqueue([] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Task 2 completed\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Main thread completed\n";


    return 0;
}
#else

void performance_test(std::size_t thread_count, std::size_t task_count) {
    ThreadPool pool(thread_count); // 创建线程池

    std::vector<std::future<void>> results; // 存储任务结果
    results.reserve(task_count);

    auto start_time = std::chrono::high_resolution_clock::now(); // 开始计时

    for (std::size_t i = 0; i < task_count; ++i) {
        results.emplace_back(pool.enqueue([]() {
            // 增加任务复杂度
            volatile int sum = 0;
            for (int j = 0; j < 100000; ++j) { // 增加循环次数
                sum += j;
            }
        }));
    }

    for (auto& result : results) {
        result.get(); // 等待所有任务完成
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // 结束计时

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "ThreadPool with " << thread_count << " threads completed "
              << task_count << " tasks in " << duration.count() << " ms." << std::endl;
}

int main() {
    std::cout << "Starting ThreadPool Performance Test...\n";

    // 控制任务量更高，观察线程池性能
    performance_test(2, 100000);
    performance_test(4, 100000);
    performance_test(8, 100000);
    performance_test(16, 100000);

    return 0;
}
#endif
