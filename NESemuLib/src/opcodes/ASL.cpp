#include "CPU.h"

#include "Assert.h"

int CPU::ASL(AddressingMode mode)
{
    int cycles = 0;
    const uint8_t value = GetValueWithMode(mode, cycles);
    const uint8_t finalValue = value << 1;
    SaveShiftOperationResult(mode, finalValue);

    SetFlag(Flag::Carry, (value & (1 << 7)) != 0);

    // Can't find a way to make the cycles return by GetValueWithMode to work with this instruction
    switch (mode)
    {
        case CPU::Accumulator:
            cycles = 2;
            break;
        case CPU::ZeroPage:
            cycles = 5;
            break;
        case CPU::ZeroPageX:
            cycles = 6;
            break;
        case CPU::Absolute:
            cycles = 6;
            break;
        case CPU::AbsoluteX:
            cycles = 7;
            break;
        default:
            OMBAssert(false, "Unsupported addressing mode!");
            break;
    }

    return cycles;
}