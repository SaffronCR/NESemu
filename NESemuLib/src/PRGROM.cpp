#include "PRGROM.h"

#include "Assert.h"
#include "SizeOfArray.h"

PRGROM::PRGROM()
{
}

PRGROM::~PRGROM()
{
}

uint8_t PRGROM::ReadMem(uint16_t address)
{
    uint16_t index = ConvertToIndex(address);
    return _prgRom[index];
}

void PRGROM::WriteMem(uint16_t address, uint8_t value)
{
    // Do nothing
    OMBAssert(false, "Trying to write ROM!!!");
}

void PRGROM::PowerOn()
{
    for (int i = 0; i < sizeofarray(_prgRom); ++i)
    {
        _prgRom[i] = 0;
    }
}

void PRGROM::Reset()
{
    // Do nothing
}

uint16_t PRGROM::ConvertToIndex(uint16_t address) const
{
    OMBAssert(address >= kStartAddress && address < kStartAddress + sizeofarray(_prgRom), "Address is not within the ROM's address space");
    if (_is16KBROM)
    {
        return (address - kStartAddress) % 16384;
    }
    else
    {
        return address - kStartAddress;
    }
}
