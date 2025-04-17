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

/**
 * @namespace Executor
 */
namespace executor {

class Executor;

/**
 * @interface Node
 * Abstract class for node object
 */
class Node {
public:
  friend Executor;

public:
  using Time = std::int64_t;
  using Frequency = float;

public:
  Node(Frequency frequency = 1000);
  virtual ~Node() = default;

public:
  /**
   * Set frequency for update node
   * @param frequency
   */
  auto setFrequency(Node::Frequency frequency) -> void;

protected:
  /** Service's method for call process() with settled frequency */
  auto spinOnce() -> void;

private:
  /** Business data implementation */
  virtual auto process() -> void = 0;

private:
  Time m_spinLastTime;
  Time m_updatePeriod;
};

} // namespace executor
