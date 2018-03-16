#pragma once

#include "MemoryHandler.h"

#include <string>

class SRAM : public MemoryHandler
{
public:
    static const int kSize = 8192;
    static const int kStartAddress = 0x6000;

    SRAM();
    virtual ~SRAM();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

    void SetEnabled(bool enabled);
    void Load(const char* path);
    void Save(const char* path);

private:
    bool _enabled = false;
    uint8_t _sram[kSize];

    uint16_t ConvertToIndex(uint16_t address) const;
};
