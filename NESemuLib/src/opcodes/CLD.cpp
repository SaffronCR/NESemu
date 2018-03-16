#include "CPU.h"

int CPU::CLD(AddressingMode mode)
{
    SetFlag(Flag::DecimalMode, false);
    return 2;
}
