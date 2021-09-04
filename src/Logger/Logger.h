//
// Created by aleksei on 3/20/21.
//

#include <ILogger.h>

#ifndef ANTONOV_LOGGER_H
#define ANTONOV_LOGGER_H
#include <iostream>
#include <fstream>

namespace {
    class Logger : public ILogger {
    public:
        static Logger *createLogger(const char* const& filename, bool overwrite = true);
        static Logger *createLogger();
        RC log(RC code, Level level, const char* const& srcfile, const char* const& function, int line) override;
        virtual RC log(RC code, Level level);
        ~Logger() override;
    private:
        explicit Logger(FILE* ofStream);
        FILE* m_fOut;
    };
}


#endif //ANTONOV_LOGGER_H
