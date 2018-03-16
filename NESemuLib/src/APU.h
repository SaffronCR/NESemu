#pragma once

#include "MemoryHandler.h"

class APU : public MemoryHandler
{
public:
    APU();
    virtual ~APU();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();
};
