#include "CPU.h"

int CPU::CLC(AddressingMode mode)
{
    SetFlag(Flag::Carry, false);
    return 2;
}
