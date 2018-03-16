#pragma once

#include "APU.h"
#include "CHRROM.h"
#include "CPU.h"
#include "Input.h"
#include "PPU.h"
#include "PRGROM.h"
#include "RAM.h"
#include "SRAM.h"

struct Hardware
{
    CPU cpu;
    RAM ram;
    PPU ppu;
    APU apu;
    Input input;
    SRAM sram;
    PRGROM prgRom;
    CHRROM chrRom;

    void PowerOn()
    {
        cpu.PowerOn();
        ram.PowerOn();
        ppu.PowerOn();
        apu.PowerOn();
        input.PowerOn();
        sram.PowerOn();
        prgRom.PowerOn();
        chrRom.PowerOn();
    }
    
    void Reset(MemoryMapper* memoryMapper, MirroringMode mirroringMode)
    {
        cpu.Reset(memoryMapper);
        ram.Reset();
        ppu.Reset(memoryMapper, &cpu, &chrRom, mirroringMode);
        apu.Reset();
        input.Reset();
        sram.Reset();
        prgRom.Reset();
        chrRom.Reset();
    }
};