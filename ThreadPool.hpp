#pragma once

#include <functional>
#include <thread>
#include <queue>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
    using Task = std::function<void()>;
public:
    ThreadPool(std::size_t thread_num = std::thread::hardware_concurrency()) {
        m_threads.reserve(thread_num);
        for (size_t i = 0; i < thread_num; ++i) {
            m_threads.emplace_back([this]() {
                executor();
            });
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool() {
        shutdown();
    }

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> {
        using ReturnType = typename std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stop) {
                throw std::runtime_error("Cannot enqueue tasks on a stopped ThreadPool");
            }
            m_tasks_queue.emplace([task]() { (*task)(); });
        }
        m_cv.notify_one();
        return result;
    }

private:
    void executor() {
        while (!m_stop.load(std::memory_order_acquire)) {

            std::unique_lock<std::mutex> lk{m_mutex};
            m_cv.wait(lk, [this] {
                return m_stop.load(std::memory_order_acquire) || !m_tasks_queue.empty();
            });

            if (!m_tasks_queue.empty()) {
                auto task = std::move(m_tasks_queue.front());
                m_tasks_queue.pop();
                lk.unlock();
                task();
            }
        }
    }

    void shutdown() {
        if (m_stop.exchange(true, std::memory_order_acq_rel) == false) {

            m_cv.notify_all();

            for (auto &thread: m_threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        }
    }

    std::vector<std::thread> m_threads;
    std::queue<Task> m_tasks_queue;

    std::mutex m_mutex;
    std::atomic<bool> m_stop {false};
    std::condition_variable m_cv;
};