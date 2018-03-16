#include "MemoryMapper.h"

MemoryMapper::MemoryMapper(Hardware& hw) : _hw(hw)
{
}

MemoryHandler& MemoryMapper::GetMemoryHandlerForAddress(uint16_t address, AccessMode mode)
{
    if (address >= 0x0 && address < 0x2000)
    {
        return _hw.ram;
    }
    else if ((address >= 0x2000 && address < 0x4000) || address == 0x4014)
    {
        return _hw.ppu;
    }
    else if ((address >= 0x4000 && address < 0x4014) || address == 0x4015 || (address == 0x4017 && mode == AccessMode::Write))
    {
        return _hw.apu;
    }
    else if (address == 0x4016 || (address == 0x4017 && mode == AccessMode::Read))
    {
        return _hw.input;
    }
    else if (address >= 0x6000 && address < 0x8000)
    {
        return _hw.sram;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        return _hw.prgRom;
    }
    else
    {
        OMBAssert(false, "Unimplemented!");
        return *(new MemoryHandler());
    }
}
