#pragma once

#include "MemoryHandler.h"

class CHRROM : public MemoryHandler
{
public:
    static const uint16_t kPageCHRROMSize = 8192;
    static const uint8_t kMaxPageCount = 4;
    static const uint16_t kMaxCHRROMSize = kMaxPageCount * kPageCHRROMSize;

    CHRROM();
    virtual ~CHRROM();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

    void SelectPage(int pageIndex);

    // For CHRROM parsing only
    uint8_t* GetCHRROMPtr() { return _chrRom; }

private:
    uint8_t _chrRom[kMaxCHRROMSize];
    int _pageIndex;
};
