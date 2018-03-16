#include "CPU.h"

int CPU::ADC(AddressingMode mode)
{
    int cycles = 1;
    const uint8_t value = GetValueWithMode(mode, cycles);
    const bool isValueNegative = IsValueNegative(value);
    const bool wasAccumulatorNegative = IsValueNegative(_accumulator);
    uint16_t result16 = _accumulator + value + (GetFlag(Flag::Carry) ? 1 : 0);
    SetAccumulator((uint8_t)result16);
    const bool isResultNegative = IsValueNegative(_accumulator);

    SetFlag(Flag::Overflow, (isValueNegative == wasAccumulatorNegative) && (isValueNegative != isResultNegative));
    SetFlag(Flag::Carry, result16 != _accumulator);

    return cycles;
}
