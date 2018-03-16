#include "CPU.h"

#include "Assert.h"

int CPU::DECINC(AddressingMode mode, int delta)
{
    int cycles = 0;
    const uint16_t programCounterBeforeGetValue = _programCounter;
    uint8_t value = GetValueWithMode(mode, cycles);
    _programCounter = programCounterBeforeGetValue;
    value += delta;
    SetValueWithMode(mode, value, cycles);
    SetFlag(Flag::Sign, IsValueNegative(value) != 0);
    SetFlag(Flag::Zero, value == 0);

    switch (mode)
    {
        case AddressingMode::ZeroPage:
            return 5;
        case AddressingMode::ZeroPageX:
            return 6;
        case AddressingMode::Absolute:
            return 6;
        case AddressingMode::AbsoluteX:
            return 7;
        default:
            OMBAssert(false, "Unsupported addressing mode");
            return 0;
    }
}
