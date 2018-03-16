#include "CPU.h"

int CPU::SEC(AddressingMode mode)
{
    SetFlag(Flag::Carry, true);
    return 2;
}
