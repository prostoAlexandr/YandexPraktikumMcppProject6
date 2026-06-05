#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "queue/unbounded_queue.hpp"

// здесь ваш код

using namespace dispatcher::queue;

TEST(UnboundedQueueTest, SingleThreadCheck) {
    UnboundedQueue uq;
    int counter = 0;
    for (int i = 0; i < 10'000; ++i) {
        uq.push([&counter] { ++counter; });
    }
    while (true) {
        auto task = uq.try_pop();
        if (task.has_value()) {
            task.value()();
        } else {
            break;
        }
        ++counter;
    }
    EXPECT_EQ(counter, 20'000);
}

TEST(UnboundedQueueTest, DoubleThreadCheck) {
    UnboundedQueue uq;
    std::jthread th([&uq] {
        for (int i = 0; i <= 100'000; ++i) {
            uq.push([i] { auto square = i * i; });
        }
    });

    int counter = 0;
    while (counter < 100'000) {
        auto task = uq.try_pop();
        if (task.has_value()) {
            ++counter;
            task.value()();
        }
    }
    EXPECT_EQ(counter, 100'000);
}

TEST(UnboundedQueueTest, MultipleSendersCheck) {
    UnboundedQueue uq;
    std::vector<std::jthread> th_vec;
    for (int i = 0; i < 100; ++i) {
        th_vec.emplace_back([&uq] {
            for (int i = 0; i <= 1'000; ++i) {
                uq.push([i] { auto square = i * i; });
            }
        });
    }

    int counter = 0;
    while (counter < 100'000) {
        auto task = uq.try_pop();
        if (task.has_value()) {
            ++counter;
            task.value()();
        }
    }
    EXPECT_EQ(counter, 100'000);
}

TEST(UnboundedQueueTest, MultipleReceiversCheck) {
    UnboundedQueue uq;
    for (int i = 0; i < 100'000; ++i) {
        uq.push([&i] { auto square = i * i; });
    }

    std::atomic<int> counter = 0;
    {
        std::vector<std::jthread> th_vec;
        for (int i = 0; i < 100; ++i) {
            th_vec.emplace_back([&counter, &uq] {
                int limit = 0;
                while (limit < 1'000) {
                    auto task = uq.try_pop();
                    if (task.has_value()) {
                        ++counter;
                        ++limit;
                        task.value()();
                    }
                }
            });
        }
    }
    EXPECT_EQ(counter, 100'000);
}