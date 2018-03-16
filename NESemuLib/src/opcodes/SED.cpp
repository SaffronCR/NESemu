#include "CPU.h"

#include "Assert.h"

int CPU::SED(AddressingMode mode)
{
    OMBAssert(false, "Decimal mode is not supported in the NES!");
    SetFlag(Flag::DecimalMode, true);
    return 2;
}
