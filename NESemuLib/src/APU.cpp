#include "APU.h"

#include "Assert.h"

APU::APU()
{
}

APU::~APU()
{
}

uint8_t APU::ReadMem(uint16_t address)
{
    OMBAssert(false, "Unimplemented");
    return 0;
}

void APU::WriteMem(uint16_t address, uint8_t value)
{
    // TODO
}

void APU::PowerOn()
{
    // TODO
}

void APU::Reset()
{
    // TODO
}
