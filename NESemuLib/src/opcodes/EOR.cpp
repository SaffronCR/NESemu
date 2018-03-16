#include "CPU.h"

int CPU::EOR(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    const uint8_t andResult = _accumulator ^ value;
    SetAccumulator(andResult);

    return cycles;
}