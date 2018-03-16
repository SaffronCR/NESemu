#pragma once

#include "MemoryHandler.h"

class PRGROM : public MemoryHandler
{
public:
    static const uint16_t kPRGROMPageSize = 16384;
    static const uint16_t kMaxPRGROMSize = 2 * kPRGROMPageSize;
    static const uint16_t kStartAddress = 0x8000;

    PRGROM();
    virtual ~PRGROM();

    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void PowerOn();
    virtual void Reset();

    // For ROM parsing only
    uint8_t* GetROMPtr() { return _prgRom; }
    void SetIs16KBROM(bool is16KROM) { _is16KBROM = is16KROM; }

private:
    uint8_t _prgRom[kMaxPRGROMSize];
    bool _is16KBROM;

    uint16_t ConvertToIndex(uint16_t address) const;
};
