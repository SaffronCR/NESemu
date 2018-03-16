#include "CPU.h"

int CPU::STX(AddressingMode mode)
{
    int cycles = 1;
    SetValueWithMode(mode, _x, cycles);

    return cycles;
}