#include "CHRROM.h"

#include "SizeOfArray.h"

CHRROM::CHRROM()
{
}

CHRROM::~CHRROM()
{
}

uint8_t CHRROM::ReadMem(uint16_t address)
{
    OMBAssert(address < sizeofarray(_chrRom), "Address is out of bounds");
    return _chrRom[(_pageIndex * kPageCHRROMSize) + address];
}

void CHRROM::WriteMem(uint16_t address, uint8_t value)
{
    // Do nothing
    OMBAssert(false, "Trying to write CHR-ROM!!!");
}

void CHRROM::PowerOn()
{
    _pageIndex = 0;
    for (int i = 0; i < sizeofarray(_chrRom); ++i)
    {
        _chrRom[i] = 0;
    }
}

void CHRROM::Reset()
{
    _pageIndex = 0;
}

void CHRROM::SelectPage(int pageIndex)
{
    _pageIndex = pageIndex;
}
