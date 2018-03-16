#include "CPU.h"

#include "Assert.h"
#include "MemoryMapper.h"

int CPU::JMP(AddressingMode mode)
{
    switch (mode)
    {
        case AddressingMode::Absolute:
        {
            // NOTE: Subtract 1 so next time PC is incremented it fetches the right opcode
            _programCounter = _memoryMapper->ReadMem(++_programCounter) + (_memoryMapper->ReadMem(++_programCounter) << 8) - 1;
            return 3;
        }
        case AddressingMode::Indirect:
        {
            const uint8_t addressLo = _memoryMapper->ReadMem(++_programCounter);
            const uint8_t addressHi = _memoryMapper->ReadMem(++_programCounter);
            const uint16_t finalAddress1 = addressLo + (addressHi << 8);
            const uint8_t addressLoPlus1 = addressLo + 1;
            const uint16_t finalAddress2 = addressLoPlus1 + (addressHi << 8);
            // NOTE: Subtract 1 so next time PC is incremented it fetches the right opcode
            _programCounter = _memoryMapper->ReadMem(finalAddress1) + (_memoryMapper->ReadMem(finalAddress2) << 8) - 1;

            return 5;
        }
        default:
        {
            OMBAssert(false, "Unsupported addressing mode!");
            return 0;
        }
    }
}
