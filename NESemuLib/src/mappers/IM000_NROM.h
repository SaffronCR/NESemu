#pragma once

#include "MemoryMapper.h"

class IM000_NROM : public MemoryMapper
{
public:
    IM000_NROM(Hardware& hw);
    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
};