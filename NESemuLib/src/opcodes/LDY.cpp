#include "CPU.h"

int CPU::LDY(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    _y = value;

    SetFlag(Flag::Sign, IsValueNegative(_y));
    SetFlag(Flag::Zero, _y == 0);

    return cycles;
}