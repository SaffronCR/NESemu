#include "CPU.h"

#include "Assert.h"

int CPU::PHP(AddressingMode mode)
{
    int cycles = 0;
    Push(_status);

    return 3;
}