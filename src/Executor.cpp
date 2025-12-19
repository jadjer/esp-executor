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

using Result = esp_err_t;

} // namespace

[[maybe_unused]] auto Executor::addNode(Node::Pointer node) -> void {
  m_nodes.push_back(std::move(node));
}

[[maybe_unused]] auto Executor::addNode(Node::Pointer node, Node::Frequency const processFrequency) -> void {
  node->setFrequency(processFrequency);

  addNode(std::move(node));
}

[[maybe_unused]] auto Executor::removeNode(Node::Pointer const& node) -> void {
  m_nodes.remove(node);
}

auto Executor::spin() -> void {
  m_enable = true;

  process();
}

auto Executor::start() -> void {
  m_enable = true;

  m_thread = Thread(&Executor::process, this);
  m_thread.detach();

  ESP_LOGI(TAG, "Executed on a separate thread");
}

auto Executor::stop() -> void {
  m_enable = false;

  if (m_thread.joinable()) {
    m_thread.join();
  }
}

auto Executor::process() -> void {
  if (m_nodes.empty()) {
    ESP_LOGW(TAG, "There are no nodes");
    return;
  }

  if (not watchdogTimerInit()) {
    ESP_LOGE(TAG, "Watchdog timer init failed");
    return;
  }

  while (m_enable) {
    if (not watchdogTimerReset()) {
      ESP_LOGE(TAG, "Watchdog timer reset failed");
      return;
    }

    for (auto const& node : m_nodes) {
      node->spinOnce();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

auto Executor::watchdogTimerInit() -> bool {
  Result const result = esp_task_wdt_add_user("executor_spin", &m_watchdogHandle);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to subscribe user");
    return false;
  }

  return true;
}

auto Executor::watchdogTimerReset() -> bool {
  Time const currentTime = esp_timer_get_time();
  Time const timeDifference = currentTime - m_watchdogResetLastTime;

  if (timeDifference < WATCHDOG_TIMER_RESET_MICROSECONDS) {
    return true;
  }

  m_watchdogResetLastTime = currentTime;

  Result const result = esp_task_wdt_reset_user(m_watchdogHandle);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to reset");
    return false;
  }

  return true;
}

} // namespace executor
