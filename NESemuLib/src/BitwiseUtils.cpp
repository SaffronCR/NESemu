#include "BitwiseUtils.h"

void BitwiseUtils::SetFlag(uint8_t& registerRef, int bitIndex, bool value)
{
    if (value)
    {
        registerRef |= (1 << bitIndex);
    }
    else
    {
        registerRef &= ~(1 << bitIndex);
    }
}

bool BitwiseUtils::IsFlagSet(uint8_t registerValue, int bitIndex)
{
    return (registerValue & (1 << bitIndex)) != 0;
}
