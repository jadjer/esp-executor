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

#include "executor/Node.hpp"

#include <esp_timer.h>

namespace executor {

namespace {

auto const DEFAULT_FREQUENCY_HZ = 1000;
auto const MICROSECONDS_PER_SECOND = 1000000;

auto convertFrequencyToPeriod(Node::Frequency const frequency) -> Node::Period {
  auto const periodInSecond = 1 / frequency;
  auto const periodInUS = static_cast<Node::Period>(periodInSecond * MICROSECONDS_PER_SECOND);

  return periodInUS;
}

} // namespace

Node::Node() noexcept : Node(DEFAULT_FREQUENCY_HZ) {}

Node::Node(Node::Frequency const frequency) noexcept : m_updatePeriod{convertFrequencyToPeriod(frequency)} {}

[[maybe_unused]] auto Node::setFrequency(Node::Frequency const frequency) -> void { m_updatePeriod = convertFrequencyToPeriod(frequency); }

auto Node::spinOnce() -> void {
  Node::Time const currentTime = esp_timer_get_time();
  Node::Time const timeDifference = currentTime - m_spinLastTime;
  if (timeDifference < m_updatePeriod) {
    return;
  }

  m_spinLastTime = currentTime;

  process();
}

} // namespace executor
