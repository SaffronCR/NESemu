#include "CPU.h"

#include "Assert.h"

int CPU::PLP(AddressingMode mode)
{
    int cycles = 0;
    _status = Pop();

    return 4;
}