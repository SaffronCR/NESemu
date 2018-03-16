#include "CPU.h"

int CPU::STA(AddressingMode mode)
{
    int cycles = 1;
    SetValueWithMode(mode, _accumulator, cycles);

    return cycles;
}