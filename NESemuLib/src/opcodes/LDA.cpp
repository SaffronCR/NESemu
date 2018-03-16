#include "CPU.h"

int CPU::LDA(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    SetAccumulator(value);

    return cycles;
}