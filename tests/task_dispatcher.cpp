#include <atomic>
#include <gtest/gtest.h>
#include <thread>

#include "task_dispatcher.hpp"
#include "types.hpp"

using namespace dispatcher;
using namespace std::chrono_literals;

// здесь ваш код
TEST(TaskDispatcherTest, DispatchingCheck) {
    TaskDispatcher td(10);
    std::atomic_int high_prio_counter = 0;
    std::atomic_int normal_prio_counter = 0;
    for (int i = 0; i < 1'000; ++i) {
        td.schedule(TaskPriority::High, [&high_prio_counter] { ++high_prio_counter; });
    }
    for (int i = 0; i < 1'000; ++i) {
        td.schedule(TaskPriority::Normal, [&normal_prio_counter] { ++normal_prio_counter; });
    }

    while (high_prio_counter < 1'000) {
        std::this_thread::sleep_for(1ms);
    }
    EXPECT_EQ(high_prio_counter, 1'000);

    while (normal_prio_counter < 1'000) {
        std::this_thread::sleep_for(1ms);
    }
    EXPECT_EQ(normal_prio_counter, 1'000);
}

TEST(TaskDispatcherTest, OrderCheck) {
    TaskDispatcher td(1);
    std::atomic_int counter = 0;

    int i = 0;
    for (; i < 1'000; ++i) {
        td.schedule(TaskPriority::High, [i, &counter] {
            EXPECT_EQ(i, counter);
            ++counter;
        });
    }
    for (; i < 2'000; ++i) {
        td.schedule(TaskPriority::Normal, [i, &counter] {
            EXPECT_EQ(i, counter);
            ++counter;
        });
    }
}

TEST(TaskDispatcherTest, DestructorCallCheck) {
    std::atomic_int counter = 0;
    {
        TaskDispatcher td(5);
        for (int i = 0; i < 10'000; ++i) {
            td.schedule(TaskPriority::Normal, [&counter] {
                ++counter;
                std::this_thread::sleep_for(10ns);
            });
        }
        EXPECT_LT(counter, 10'000);
    }
    EXPECT_EQ(counter, 10'000);
}