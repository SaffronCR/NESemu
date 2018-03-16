#include "CPU.h"

#include "Assert.h"
#include "MemoryMapper.h"

int CPU::JSR(AddressingMode mode)
{
    OMBAssert(mode == AddressingMode::Absolute, "Only supported addressing mode is Absolute");

    uint16_t nextOpAddress = _programCounter + 2;
    Push(GetHighByte(nextOpAddress));
    Push(GetLowByte(nextOpAddress));
    // NOTE: Subtract 1 so next time PC is incremented it fetches the right opcode
    _programCounter = _memoryMapper->ReadMem(++_programCounter) + (_memoryMapper->ReadMem(++_programCounter) << 8) - 1;
    return 6;
}
