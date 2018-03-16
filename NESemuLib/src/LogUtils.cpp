#include "LogUtils.h"

#include <cstdarg>
#include <cstdio>
#include <string>
#include <Windows.h>

void Log::Info(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int bufferSize = 1024;
    char buff[bufferSize];
    vsnprintf(buff, bufferSize, fmt, args);
    printf(buff);
    printf("\n");
    va_end(args);
}

void Log::Debug(const char* fmt, ...)
{
#if _DEBUG
    va_list args;
    va_start(args, fmt);
    const int bufferSize = 1024;
    char buff[bufferSize];
    printf("[Debug]");
    vsnprintf(buff, bufferSize, fmt, args);
    printf(buff);
    printf("\n");
    va_end(args);
#endif
}

void Log::Warning(const char* fmt, ...)
{
#if _DEBUG
    va_list args;
    va_start(args, fmt);
    const int bufferSize = 1024;
    char buff[bufferSize];
    printf("[Warning]");
    vsnprintf(buff, bufferSize, fmt, args);
    printf(buff);
    printf("\n");
    va_end(args);
#endif
}

void Log::Error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int bufferSize = 1024;
    char buff[bufferSize];
    printf("[Error]");
    vsnprintf(buff, bufferSize, fmt, args);
    printf(buff);
    printf("\n");
    va_end(args);
}
