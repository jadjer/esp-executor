// Copyright 2025 Pavel Suprunov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "executor/Executor.hpp"

#include <chrono>
#include <esp_err.h>
#include <esp_task_wdt.h>
#include <esp_timer.h>
#include <thread>
#include <utility>

#include "executor/Node.hpp"

namespace executor {

auto const MICROSECONDS_PER_SECOND = 1000000;
auto const WATCHDOG_TIMER_RESET_MICROSECONDS = 3 * MICROSECONDS_PER_SECOND;

void Executor::addNode(Executor::NodePtr node) { m_nodes.push_back(std::move(node)); }

void Executor::addNode(Executor::NodePtr node, Node::Frequency const frequencyInHz) {
  node->setFrequency(frequencyInHz);

  addNode(std::move(node));
}

[[maybe_unused]] void Executor::removeNode(NodePtr const &node) { m_nodes.remove(node); }

[[noreturn]] void Executor::spin() {
  ESP_ERROR_CHECK(esp_task_wdt_add_user("executor_spin", &m_watchdogHandle));

  while (true) {
    watchdogTimerReset();

    for (auto const &node : m_nodes) {
      node->spinOnce();
    }

    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  ESP_ERROR_CHECK(esp_task_wdt_delete_user(m_watchdogHandle));
}

void Executor::watchdogTimerReset() {
  auto const currentTime = esp_timer_get_time();
  auto const timeDifference = currentTime - m_watchdogResetLastTime;
  if (timeDifference < WATCHDOG_TIMER_RESET_MICROSECONDS) {
    return;
  }

  m_watchdogResetLastTime = timeDifference;

  ESP_ERROR_CHECK(esp_task_wdt_reset_user(m_watchdogHandle));
}

} // namespace executor
