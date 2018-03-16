#include "CPU.h"

int CPU::CLI(AddressingMode mode)
{
    SetFlag(Flag::InterruptDisable, false);
    return 2;
}
