#pragma once

#include <cstdio>

// Assert function inspired by http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
#if !_DEBUG

#define OMBAssert(condition, errmsg, ...) do { (void)sizeof(condition); } while(0)

#else

#define OMBAssert(condition, errmsg, ...) \
do \
{ \
    if (!(condition)) \
    { \
        std::printf("\n%s:%d: Assertion failed: ", (__FILE__), (__LINE__)); \
        std::printf(errmsg, ##__VA_ARGS__); \
        std::printf("\r"); \
        __asm int 3 \
    } \
} while(0) \

#endif