#pragma once

#include <stdint.h>

namespace BitwiseUtils
{
    void SetFlag(uint8_t& registerRef, int bitIndex, bool value);
    bool IsFlagSet(uint8_t registerValue, int bitIndex);
}
