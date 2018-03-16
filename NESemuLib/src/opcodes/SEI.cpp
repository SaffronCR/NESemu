#include "CPU.h"

int CPU::SEI(AddressingMode mode)
{
    SetFlag(Flag::InterruptDisable, true);
    return 2;
}
