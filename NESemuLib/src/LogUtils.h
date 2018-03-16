#pragma once

namespace Log
{
    void Info(const char* fmt, ...);
    void Debug(const char* fmt, ...);
    void Warning(const char* fmt, ...);
    void Error(const char* fmt, ...);
}