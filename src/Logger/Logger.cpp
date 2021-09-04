//
// Created by aleksei on 3/20/21.
//

#include "Logger.h"
Logger * Logger::createLogger(const char *const &filename, bool overwrite) {
    FILE* fout = std::fopen(filename, overwrite ? "w" : "a" );
    if(fout != nullptr){
        return new Logger(fout);
    }
    return nullptr;
}

Logger::Logger(FILE* ofFile) {
    m_fOut = ofFile;
}

Logger::~Logger() {
    if(m_fOut != stdout){
        fclose(m_fOut);
    }
}

RC Logger::log(RC code, ILogger::Level level, const char *const &srcfile, const char *const &function, int line) {
    fprintf(m_fOut, "File: %s, function: %s, line = %i\n", srcfile, function, line);
    Logger::log(code, level);
    return RC::SUCCESS;
}

Logger *Logger::createLogger() {
    return new Logger(stdout);
}

RC Logger::log(RC code, ILogger::Level level) {
    switch (level){

        case Level::SEVERE:
            fprintf(m_fOut, "ERROR ");
            break;
        case Level::WARNING:
            fprintf(m_fOut, "WARNING ");
            break;
        case Level::INFO:
            fprintf(m_fOut, "INFO ");
            break;
    }
    switch (code) {

        case RC::UNKNOWN:
            fprintf(m_fOut, "Unknown");
            break;
        case RC::SUCCESS:
            fprintf(m_fOut, "Success");
            break;
        case RC::INVALID_ARGUMENT:
            fprintf(m_fOut, "Invalid arguments");
            break;
        case RC::MISMATCHING_DIMENSIONS:
            fprintf(m_fOut, "Dimensions are not equal");
            break;
        case RC::INDEX_OUT_OF_BOUND:
            fprintf(m_fOut, "Index is out of bound");
            break;
        case RC::INFINITY_OVERFLOW:
            fprintf(m_fOut, "Infinity overflow");
            break;
        case RC::NOT_NUMBER:
            fprintf(m_fOut, "NAN");
            break;
        case RC::ALLOCATION_ERROR:
            fprintf(m_fOut, "ALLOCATION_ERROR");
            break;
        case RC::NULLPTR_ERROR:
            fprintf(m_fOut, "NULLPTR_ERROR");
            break;
        case RC::FILE_NOT_FOUND:
            fprintf(m_fOut, "FILE_NOT_FOUND");
            break;
        case RC::VECTOR_NOT_FOUND:
            fprintf(m_fOut, "VECTOR_NOT_FOUND");
            break;
        case RC::IO_ERROR:
            fprintf(m_fOut, "IO_ERROR");
            break;
        case RC::MEMORY_INTERSECTION:
            fprintf(m_fOut, "MEMORY_INTERSECTION");
            break;
        case RC::AMOUNT:
            fprintf(m_fOut, "AMOUNT");
            break;
        case RC::SOURCE_SET_DESTROYED:
            fprintf(m_fOut, "SOURCE_SET_DESTROYED");
            break;
        case RC::SOURCE_COMPACT_DESTROYED:
            fprintf(m_fOut, "SOURCE_COMPACT_DESTROYED");
            break;
        case RC::SOURCE_SET_EMPTY:
            fprintf(m_fOut, "SOURCE_SET_EMPTY");
            break;
        case RC::VECTOR_ALREADY_EXIST:
            fprintf(m_fOut, "VECTOR_ALREADY_EXIST");
            break;
        case RC::SET_INDEX_OVERFLOW:
            fprintf(m_fOut, "SET_INDEX_OVERFLOW");
            break;
        case RC::NO_ARGS_SET:
            fprintf(m_fOut, "NO_ARGS_SET");
            break;
        case RC::NO_PARAMS_SET:
            fprintf(m_fOut, "NO_PARAMS_SET");
            break;
        case RC::NO_PROBLEM_SET:
            fprintf(m_fOut, "NO_PROBLEM_SET");
            break;
    }
    fprintf(m_fOut, "\n");
    return RC::SUCCESS;
}

ILogger::~ILogger() = default;

ILogger * ILogger::createLogger(){
    return Logger::createLogger();
}

ILogger * ILogger::createLogger(const char* const& filename, bool overwrite){
    return Logger::createLogger(filename, overwrite);
}


