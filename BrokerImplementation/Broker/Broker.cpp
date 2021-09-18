//
// Created by aleksei on 18.09.2021.
//

#include "Broker.h"
#include "IDiffProblem.h"
#include "ISolver.h"
#include <new>

Broker* Broker::m_broker = nullptr;

void* getBroker() {
  if(Broker::m_broker == nullptr) {
    Broker::m_broker = new(std::nothrow) Broker();
  }
  return Broker::m_broker;
}

bool Broker::canCastTo(INTERFACE_IMPL impl) const {
  return impl == INTERFACE_IMPL::IPROBLEM || impl == INTERFACE_IMPL::ISOLVER;
}

void* Broker::getInterfaceImpl(INTERFACE_IMPL impl) const {
  switch (impl){
    case INTERFACE_IMPL::IPROBLEM:
      return IDiffProblem::createDiffProblem();
    case INTERFACE_IMPL::ISOLVER:
      return ISolver::createSolver();
    default:
      return nullptr;
  }
}

void Broker::release() {
  delete m_broker;
  m_broker = nullptr;
}

Broker::~Broker() = default;

IBroker::~IBroker() = default;
