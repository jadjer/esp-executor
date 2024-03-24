// Copyright 2024 Pavel Suprunov
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

//
// Created by jadjer on 15.08.23.
//

#include "executor/Executor.hpp"

namespace executor {

Executor::Executor() : m_enable(true), m_nodes() {}

Executor::~Executor() {
  m_enable = false;
}

void Executor::addNode(NodePtr const &node) {
  m_nodes.push_back(node);
}

void Executor::removeNode(NodePtr const &node) {
  m_nodes.remove(node);
}

void Executor::spin() {
  while (m_enable) {
    for (auto const &node : m_nodes) {
      node->spinOnce();
    }
  }
}

}
