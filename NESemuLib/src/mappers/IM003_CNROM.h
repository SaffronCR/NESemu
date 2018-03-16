#pragma once

#include "MemoryMapper.h"

class IM003_CNROM : public MemoryMapper
{
public:
    IM003_CNROM(Hardware& hw);
    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
};
