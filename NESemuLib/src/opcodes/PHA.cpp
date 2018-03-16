#include "CPU.h"

#include "Assert.h"

int CPU::PHA(AddressingMode mode)
{
    int cycles = 0;
    Push(_accumulator);

    return 3;
}