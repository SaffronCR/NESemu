#include "SRAM.h"

#include "Assert.h"
#include "SizeOfArray.h"

#include <stdio.h>

SRAM::SRAM()
{
}

SRAM::~SRAM()
{
}

uint8_t SRAM::ReadMem(uint16_t address)
{
    if (_enabled)
    {
        uint16_t index = ConvertToIndex(address);
        return _sram[index];
    }
    else
    {
        return 0;
    }
}

void SRAM::WriteMem(uint16_t address, uint8_t value)
{
    if (_enabled)
    {
        uint16_t index = ConvertToIndex(address);
        _sram[index] = value;
    }
}

void SRAM::PowerOn()
{
    for (int i = 0; i < sizeofarray(_sram); ++i)
    {
        _sram[i] = 0;
    }
    _enabled = false;
}

void SRAM::Reset()
{
    // Do nothing
}

void SRAM::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

void SRAM::Load(const char* path)
{
    FILE* file;
    fopen_s(&file, path, "rb");
    fread_s(_sram, sizeof(_sram), sizeof(uint8_t), sizeofarray(_sram), file);
    fclose(file);
    _enabled = true;
}

void SRAM::Save(const char* path)
{
    FILE* file;
    fopen_s(&file, path, "wb");
    fwrite(_sram, sizeof(uint8_t), sizeof(_sram), file);
    fclose(file);
}

uint16_t SRAM::ConvertToIndex(uint16_t address) const
{
    if (address >= kStartAddress && address < kStartAddress + kSize)
    {
        return address - kStartAddress;
    }
    else
    {
        OMBAssert(false, "Address is not within the SRAM's address space");
        return 0;
    }
}
