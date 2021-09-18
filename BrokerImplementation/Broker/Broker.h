//
// Created by aleksei on 18.09.2021.
//

#ifndef TESTS_BROKER_H
#define TESTS_BROKER_H
#include "IBroker.h"
namespace
{
  class Broker : public IBroker
  {
  public:
    static Broker* m_broker;
  public:
    bool canCastTo(INTERFACE_IMPL impl) const override;
    void * getInterfaceImpl(INTERFACE_IMPL impl) const override;
    void release() override;

  private:
    ~Broker() override;
  };
}


#endif //TESTS_BROKER_H
