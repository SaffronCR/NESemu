#include "CPU.h"

int CPU::DEY(AddressingMode mode)
{
    --_y;
    SetFlag(Flag::Sign, IsValueNegative(_y));
    SetFlag(Flag::Zero, _y == 0);
    return 2;
}
