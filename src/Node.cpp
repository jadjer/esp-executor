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

auto const MICROSECONDS_IN_SECOND = 1000000;

Node::Time convertFrequencyToPeriod(Node::Frequency const frequency) {
  auto const period_InSecond = 1 / frequency;
  auto const period_InUS = static_cast<Node::Time>(period_InSecond * MICROSECONDS_IN_SECOND);

  return period_InUS;
}

Node::Node(Node::Frequency const frequency) : m_spinLastTime(0),
                                              m_updatePeriod(convertFrequencyToPeriod(frequency)) {}

void Node::setFrequency(Node::Frequency const frequency) {
  m_updatePeriod = convertFrequencyToPeriod(frequency);
}

void Node::spinOnce() {
  auto const currentTime = esp_timer_get_time();
  auto const timeDifference = currentTime - m_spinLastTime;
  if (timeDifference < m_updatePeriod) {
    return;
  }

  m_spinLastTime = currentTime;

  process();
}

}// namespace executor
