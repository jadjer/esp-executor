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

#pragma once

#include <atomic>
#include <cstdint>
#include <esp_task_wdt.h>
#include <executor/Node.hpp>
#include <list>
#include <memory>
#include <thread>

/**
 * @namespace Executor
 */
namespace executor {

/**
 * @class Executor
 * Вызывает метод process у Node с заданной частотой
 */
class Executor {
private:
  using Flag = std::atomic_bool;
  using Time = std::int64_t;
  using Nodes = std::list<Node::Pointer>;
  using Thread = std::thread;
  using WatchDogHandle = esp_task_wdt_user_handle_t;

public:
  [[maybe_unused]] auto addNode(Node::Pointer node) -> void;
  [[maybe_unused]] auto addNode(Node::Pointer node, Node::Frequency processFrequency) -> void;
  [[maybe_unused]] auto removeNode(Node::Pointer const &node) -> void;

public:
  auto spin() -> void;

public:
  auto start() -> void;
  auto stop() -> void;

private:
  auto process() -> void;
  auto watchdogTimerInit() -> bool;
  auto watchdogTimerReset() -> bool;

private:
  Flag m_enable{false};
  Nodes m_nodes{};
  Thread m_thread{};
  Time m_watchdogResetLastTime{0};
  WatchDogHandle m_watchdogHandle{nullptr};
};

} // namespace executor
