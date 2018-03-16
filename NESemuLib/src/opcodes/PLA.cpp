#include "CPU.h"

#include "Assert.h"

int CPU::PLA(AddressingMode mode)
{
    int cycles = 0;
    SetAccumulator(Pop());

    return 4;
}