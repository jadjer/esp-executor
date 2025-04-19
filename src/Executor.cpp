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
#include <esp_log.h>
#include <esp_task_wdt.h>
#include <esp_timer.h>
#include <thread>
#include <utility>

#include "executor/Node.hpp"

namespace executor {

namespace {

auto const TAG = "Executor";

auto const MICROSECONDS_PER_SECOND = 1000000;
auto const WATCHDOG_TIMER_RESET_MICROSECONDS = 3 * MICROSECONDS_PER_SECOND;

using Error = esp_err_t;

} // namespace

[[maybe_unused]] auto Executor::addNode(Executor::NodePtr node) -> void { nodes.push_back(std::move(node)); }

[[maybe_unused]] auto Executor::addNode(Executor::NodePtr node, Node::Frequency const processFrequency) -> void {
  node->setFrequency(processFrequency);

  addNode(std::move(node));
}

[[maybe_unused]] auto Executor::removeNode(Executor::NodePtr const &node) -> void { nodes.remove(node); }

auto Executor::spin() -> void {
  if (not watchdogTimerInit()) {
    return;
  }

  while (true) {
    if (not watchdogTimerReset()) {
      return;
    }

    for (auto const &node : nodes) {
      node->spinOnce();
    }

    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
}

auto Executor::watchdogTimerInit() -> bool {
  Error const result = esp_task_wdt_add_user("executor_spin", &watchdogHandle);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to subscribe user");
    return false;
  }

  return true;
}

auto Executor::watchdogTimerReset() -> bool {
  auto const currentTime = esp_timer_get_time();
  auto const timeDifference = currentTime - watchdogResetLastTime;
  if (timeDifference < WATCHDOG_TIMER_RESET_MICROSECONDS) {
    return true;
  }

  watchdogResetLastTime = timeDifference;

  Error const result = esp_task_wdt_reset_user(watchdogHandle);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to reset");
    return false;
  }

  return true;
}

} // namespace executor
