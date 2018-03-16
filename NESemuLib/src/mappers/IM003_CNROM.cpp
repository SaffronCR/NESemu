#include "IM003_CNROM.h"

IM003_CNROM::IM003_CNROM(Hardware& hw) : MemoryMapper(hw)
{
}

uint8_t IM003_CNROM::ReadMem(uint16_t address)
{
    MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Read);
    return handler.ReadMem(address);
}

void IM003_CNROM::WriteMem(uint16_t address, uint8_t value)
{
    if (address >= 0x8000 && address <= 0xFFFF)
    {
        int page = value & 0x3;
        _hw.chrRom.SelectPage(page);
    }
    else
    {
        MemoryHandler& handler = GetMemoryHandlerForAddress(address, AccessMode::Write);
        handler.WriteMem(address, value);
    }
}
