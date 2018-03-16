#pragma once

#include "Assert.h"
#include "Hardware.h"

#include <stdint.h>

class MemoryMapper
{
public:
    enum AccessMode
    {
        Read,
        Write
    };

    MemoryMapper(Hardware& hw);
    virtual ~MemoryMapper() {}
    virtual uint8_t ReadMem(uint16_t address) = 0;
    virtual void WriteMem(uint16_t address, uint8_t value) {}

protected:
    Hardware& _hw;

    MemoryHandler& GetMemoryHandlerForAddress(uint16_t address, AccessMode mode);
};
