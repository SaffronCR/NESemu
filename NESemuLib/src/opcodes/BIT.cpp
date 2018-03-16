#include "CPU.h"

int CPU::BIT(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    const uint8_t andResult = _accumulator & value;

    SetFlag(Flag::Zero, andResult == 0);
    SetFlag(Flag::Sign, (value & (1 << 7)) != 0);
    SetFlag(Flag::Overflow, (value & (1 << 6)) != 0);

    return cycles;
}