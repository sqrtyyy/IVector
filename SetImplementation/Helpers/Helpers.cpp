//
// Created by aleksei on 22.08.2021.
//

#include "Helpers.h"

bool checkPointer(ILogger* logger, void* ptr, bool isLog){
  if (ptr == nullptr){
    if (isLog) LogError(logger, RC::NULLPTR_ERROR);
    return false;
  }
  return true;
}