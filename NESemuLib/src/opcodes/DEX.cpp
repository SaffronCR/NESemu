#include "CPU.h"

int CPU::DEX(AddressingMode mode)
{
    --_x;
    SetFlag(Flag::Sign, IsValueNegative(_x));
    SetFlag(Flag::Zero, _x == 0);
    return 2;
}
