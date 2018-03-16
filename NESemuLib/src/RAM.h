#pragma once

#include "MemoryHandler.h"

class RAM : public MemoryHandler
{
public:
    static const int kRAMSize = 2048;

    RAM();
    virtual ~RAM();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

private:
    uint8_t _ram[kRAMSize];
    
    uint16_t ConvertToRealAddress(uint16_t address) const;
};
