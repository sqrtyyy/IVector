//
// Created by aleksei on 22.08.2021.
//

#ifndef UI_MATH_HELPERS_H
#define UI_MATH_HELPERS_H

#include "ILogger.h"

#define LogError(Logger, Code)  if((Logger) != nullptr) (Logger)->warning((Code), __FILE__, __func__, __LINE__)


bool checkPointer(ILogger* logger, void* ptr, bool isLog = true);

#endif //UI_MATH_HELPERS_H
