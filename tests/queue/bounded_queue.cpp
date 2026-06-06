#include <gtest/gtest.h>

#include "queue/bounded_queue.hpp"
#include <thread>

using namespace std::chrono_literals;
using namespace dispatcher::queue;

// здесь ваш код
TEST(BoundedQueue, SingleThreadCheck) {
    BoundedQueue bq(1'000);
    int counter = 0;
    for (int i = 0; i < 1'000; ++i) {
        bq.push([&counter] { ++counter; });
    }

    while (true) {
        auto task = bq.try_pop();
        if (task.has_value()) {
            task.value()();
        } else {
            break;
        }
        ++counter;
    }
    EXPECT_EQ(counter, 2'000);
}

TEST(BoundedQueueTest, DoubleThreadCheck) {
    BoundedQueue bq(1'000);
    std::atomic<int> counter = 0;
    std::jthread th([&bq, &counter] {
        for (int i = 0; i <= 100'000; ++i) {
            bq.push([&counter] { ++counter; });
        }
    });

    int limit = 0;
    while (limit < 100'000) {
        auto task = bq.try_pop();
        if (task.has_value()) {
            ++counter;
            ++limit;
            task.value()();
        }
    }
    EXPECT_EQ(counter, 200'000);
}

TEST(BoundedQueueTest, MultipleSendersCheck) {
    BoundedQueue bq(1'000);
    int counter = 0;
    std::vector<std::jthread> th_vec;
    for (int i = 0; i < 100; ++i) {
        th_vec.emplace_back([&bq, &counter] {
            for (int i = 0; i <= 1'000; ++i) {
                bq.push([&counter] { ++counter; });
            }
        });
    }

    int limit = 0;
    while (limit < 100'000) {
        auto task = bq.try_pop();
        if (task.has_value()) {
            ++counter;
            ++limit;
            task.value()();
        }
    }
    EXPECT_EQ(counter, 200'000);
}

TEST(BoundedQueueTest, MultipleReceiversCheck) {
    BoundedQueue bq(1'000);
    std::atomic<int> counter = 0;

    {
        std::vector<std::jthread> th_vec;
        for (int i = 0; i < 10; ++i) {
            th_vec.emplace_back([&counter, &bq] {
                int limit = 0;
                while (limit < 10'000) {
                    auto task = bq.try_pop();
                    if (task.has_value()) {
                        ++counter;
                        ++limit;
                        task.value()();
                    }
                }
            });
        }

        for (int i = 0; i < 100'000; ++i) {
            bq.push([&counter] { ++counter; });
        }
    }
    EXPECT_EQ(counter, 200'000);
}

TEST(BoundedQueueTest, MultiRecMultiSendCheck) {
    BoundedQueue bq(1'000);
    std::vector<std::jthread> th_vec;
    std::atomic<int> counter = 0;
    for (int i = 0; i < 100; ++i) {
        th_vec.emplace_back([&bq, &counter] {
            for (int i = 0; i <= 1'000; ++i) {
                bq.push([&counter] { ++counter; });
            }
        });
    }

    {
        std::vector<std::jthread> th_vec;
        for (int i = 0; i < 100; ++i) {
            th_vec.emplace_back([&counter, &bq] {
                int limit = 0;
                while (limit < 1'000) {
                    auto task = bq.try_pop();
                    if (task.has_value()) {
                        ++counter;
                        ++limit;
                        task.value()();
                    }
                }
            });
        }
    }
    EXPECT_EQ(counter, 200'000);
}

TEST(BoundedQueueTest, BoundCheck) {
    BoundedQueue bq(1'000);
    std::atomic_int counter = 0;
    std::jthread pusher([&bq, &counter] {
        for (int i = 0; i < 2'000; ++i) {
            bq.push([&i] { auto result = i * i; });
            ++counter;
        }
    });

    while (counter < 1'000) {
        std::this_thread::sleep_for(1ms);
    }
    EXPECT_EQ(counter, 1'000);
    for (int i = 0; i < 2'000; ++i) {
        bq.try_pop();
    }
}