#include "CPU.h"

int CPU::CLV(AddressingMode mode)
{
    SetFlag(Flag::Overflow, false);
    return 2;
}
