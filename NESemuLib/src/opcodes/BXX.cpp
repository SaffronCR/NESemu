#include "CPU.h"

#include "MemoryMapper.h"

int CPU::BXX(AddressingMode mode, bool condition)
{
    OMBAssert(mode == AddressingMode::Relative, "Only Relative addressing mode is supported on branch instructions!");
    const uint8_t relativeAddress = _memoryMapper->ReadMem(++_programCounter);

    int cycles = 2;
    if (condition)
    {
        ++cycles;
        const uint8_t oldPCHigh = GetHighByte(_programCounter);
        if (relativeAddress < 0x80)
        {
            _programCounter += relativeAddress;
        }
        else
        {
            const uint8_t relativeAddressInverted = ~relativeAddress;
            _programCounter -= (relativeAddressInverted + 1);
        }

        const uint8_t newPCHigh = GetHighByte(_programCounter);
        if (oldPCHigh != newPCHigh)
        {
            // +1 cycle when crossing page boundary
            ++cycles;
        }
    }

    return cycles;
}