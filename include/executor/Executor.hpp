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
#include <expected>
#include <list>
#include <memory>

/**
 * @namespace Executor
 */
namespace executor {

/**
 * @class Executor
 * Вызывает метод process у Node с заданной частотой
 */
class Executor {
public:
  using NodePtr = std::shared_ptr<Node>;

private:
  using Time = std::int64_t;
  using Nodes = std::list<NodePtr>;
  using WatchDogHandle = esp_task_wdt_user_handle_t;

public:
  [[maybe_unused]] auto addNode(NodePtr node) -> void;
  [[maybe_unused]] auto addNode(NodePtr node, Node::Frequency processFrequency) -> void;
  [[maybe_unused]] auto removeNode(NodePtr const &node) -> void;

public:
  auto spin() -> void;

private:
  auto watchdogTimerInit() -> bool;
  auto watchdogTimerReset() -> bool;

private:
  Nodes nodes{};
  Time watchdogResetLastTime{0};
  WatchDogHandle watchdogHandle{nullptr};
};

} // namespace executor
