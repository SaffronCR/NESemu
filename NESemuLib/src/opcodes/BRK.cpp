#include "CPU.h"

#include "MemoryMapper.h"

int CPU::BRK(AddressingMode mode)
{
    const uint16_t pcPlus1 = _programCounter + 1;
    const uint8_t statusPlusBFlag = _status | (1 << Flag::Break) | (1 << Flag::Unused);
    Push(GetHighByte(pcPlus1));
    Push(GetLowByte(pcPlus1));
    Push(statusPlusBFlag);
    SetFlag(Flag::InterruptDisable, true);
    _programCounter = _memoryMapper->ReadMem(kInterruptBreakVectorAddressL) +
        (_memoryMapper->ReadMem(kInterruptBreakVectorAddressH) << 8) - 1;

    return 7;
}