#include <atomic>
#include <gtest/gtest.h>
#include <optional>
#include <stdexcept>
#include <thread>

#include "queue/priority_queue.hpp"
#include "types.hpp"

using namespace dispatcher;
using namespace dispatcher::queue;
using namespace std::chrono_literals;

// здесь ваш код
TEST(PriorityQueueTest, PriorityCheck) {
    PriorityQueue pq(
        {{.bounded = true, .capacity = 1'000}, {.bounded = false, .capacity = std::nullopt}});
    int high_prio_counter = 0;
    int normal_prio_counter = 0;
    for (int i = 0; i < 1'000; ++i) {
        pq.push(TaskPriority::High, [&high_prio_counter] { ++high_prio_counter; });
    }
    for (int i = 0; i < 2'000; ++i) {
        pq.push(TaskPriority::Normal, [&normal_prio_counter] { ++normal_prio_counter; });
    }

    for (int i = 0; i < 1'000; ++i) {
        auto task = pq.pop();
        if (task.has_value()) {
            task.value()();
        }
    }
    EXPECT_EQ(high_prio_counter, 1'000);

    for (int i = 0; i < 2'000; ++i) {
        auto task = pq.pop();
        if (task.has_value()) {
            task.value()();
        }
    }
    EXPECT_EQ(normal_prio_counter, 2'000);
}

TEST(PriorityQueueTest, ConfigCheck) {
    EXPECT_THROW(PriorityQueue pq({{.bounded = true, .capacity = std::nullopt},
                                   {.bounded = false, .capacity = std::nullopt}}),
                 ConfigurationError);
    EXPECT_THROW(PriorityQueue pq({{.bounded = false, .capacity = std::nullopt}}),
                 ConfigurationError);
}

TEST(PriorityQueueTest, BlockOnPopCheck) {
    PriorityQueue pq(
        {{.bounded = true, .capacity = 1'000}, {.bounded = false, .capacity = std::nullopt}});
    std::atomic_bool pop_thread_finished = false;
    std::atomic_int counter = 0;

    std::jthread pop_thread([&pq, &pop_thread_finished, &counter] {
        while (true) {
            auto task = pq.pop();
            if (task.has_value()) {
                task.value()();
                ++counter;
            } else {
                break;
            }
        }
        pop_thread_finished = true;
    });

    for (int i = 0; i < 10'000; ++i) {
        pq.push(TaskPriority::Normal, [&counter] { ++counter; });
    }
    while (counter < 20'000) {
        std::this_thread::sleep_for(1ms);
    }

    EXPECT_FALSE(pop_thread_finished);
    EXPECT_EQ(counter, 20'000);

    pq.shutdown();
    std::this_thread::sleep_for(1ms);
    EXPECT_TRUE(pop_thread_finished);
}