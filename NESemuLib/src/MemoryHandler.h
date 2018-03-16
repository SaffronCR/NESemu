#pragma once

#include "Assert.h"

#include <stdint.h>

class MemoryHandler
{
public:
    virtual ~MemoryHandler() {}
    virtual uint8_t ReadMem(uint16_t address) { OMBAssert(false, "Don't use the MemoryHandler class directly!"); return 0; }
    virtual void WriteMem(uint16_t address, uint8_t value) {}

    // According to nestech.txt, PowerOn and Reset should so different things (i.e. RAM is *NOT* zeroed when doing a reset)
    virtual void PowerOn() {}
    virtual void Reset() {}
};