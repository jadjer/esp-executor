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

#include <cstdint>
#include <esp_task_wdt.h>
#include <executor/Node.hpp>
#include <list>
#include <memory>

/**
 * @namespace Executor
 */
namespace executor {

/**
 * @class Executor
 */
class Executor {
public:
  using Time = std::int64_t;
  using NodePtr = std::unique_ptr<Node>;
  using Nodes = std::list<NodePtr>;
  using WatchDogHandle = esp_task_wdt_user_handle_t;

public:
  /**
   * Node add to list
   * @param node Node ptr
   */
  [[maybe_unused]] auto addNode(NodePtr node) -> void;

  /**
   * Node add to list
   * @param node Node ptr
   * @param frequency Frequency
   */
  [[maybe_unused]] auto addNode(NodePtr node, Node::Frequency frequencyInHz) -> void;

  /**
   * Node remove from list
   * @param node Node ptr
   */
  [[maybe_unused]] auto removeNode(NodePtr const &node) -> void;

public:
  /**
   * Loop executor
   */
  [[noreturn]] auto spin() -> void;

private:
  /**
   * Reset watchdog timer every 3 seconds
   */
  auto watchdogTimerReset() -> void;

private:
  Nodes m_nodes = {};
  Time m_watchdogResetLastTime = 0;
  WatchDogHandle m_watchdogHandle = nullptr;
};

} // namespace executor
