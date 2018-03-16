#pragma once

#include "MirroringMode.h"

#include <stdint.h>

class CHRROM;
class PRGROM;

class INESParser
{
public:
    ~INESParser();
    void Parse(const char* path, PRGROM& prgRom, CHRROM& chrRom);

    void PrintInfo() const;
    uint8_t GetPRGROMPageCount() const;
    uint8_t GetCHRROMPageCount() const;
    MirroringMode GetMirroringMode() const;
    bool IsSRAMEnabled() const;
    bool IsTrainerPresent() const;
    bool IsFourScreenMirroringEnabled() const;
    int GetMapperNumber() const;

private:
    bool _loaded = false;
    uint8_t _header[16];
};