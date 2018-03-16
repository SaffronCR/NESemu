#include "NESemu.h"

#include "SizeOfArray.h"

#include "mappers\IM000_NROM.h"
#include "mappers\IM003_CNROM.h"

NESemu::NESemu()
{
    _hw.PowerOn();
}

NESemu::~NESemu()
{
}

void NESemu::Load(const char* path)
{
    _parser.Parse(path, _hw.prgRom, _hw.chrRom);
    _parser.PrintInfo();
    _hw.sram.SetEnabled(_parser.IsSRAMEnabled());

    if (_mapper != nullptr)
    {
        delete _mapper;
    }

    switch (_parser.GetMapperNumber())
    {
        case 0:
            _mapper = new IM000_NROM(_hw);
            break;
        case 3:
            _mapper = new IM003_CNROM(_hw);
            break;
        default:
            OMBAssert(false, "Unsupported memory mapper #%03d", _parser.GetMapperNumber());
            _mapper = nullptr;
    }

    Reset();
}

void NESemu::Load(const uint8_t rom[], uint16_t romSize)
{
    OMBAssert(romSize == PRGROM::kMaxPRGROMSize || romSize == PRGROM::kMaxPRGROMSize / 2, "Unsupported ROM size");
    _mapper = new IM000_NROM(_hw);
    for (int i = 0; i < romSize; ++i)
    {
        _hw.prgRom.GetROMPtr()[i] = rom[i];
    }
    _hw.prgRom.SetIs16KBROM(romSize == PRGROM::kMaxPRGROMSize / 2);

    Reset();
}

void NESemu::Load(const uint8_t rom[], uint16_t romSize, const uint8_t chrRom[], uint16_t chrRomSize)
{
    OMBAssert(chrRomSize == CHRROM::kMaxCHRROMSize, "Unsupported CHR-ROM size");
    _mapper = new IM000_NROM(_hw);
    for (int i = 0; i < chrRomSize; ++i)
    {
        _hw.chrRom.GetCHRROMPtr()[i] = chrRom[i];
    }

    Load(rom, romSize);
}

void NESemu::Update(double delta)
{
// Prevent big deltas while debugging (otherwise we might get stuck here for too long)
#if _DEBUG
    if (delta > 1.0f / 30)
    {
        delta = 1.0f / 30;
    }
#endif

    const double masterDelta = kMasterClockSpeed * delta;
    const uint64_t ppuDelta = (uint64_t)std::round(masterDelta / 4); // The PPU runs 4x slower than the master clock

    for (int i = 0; i < ppuDelta; ++i)
    {
        if (i % 3 == 0)
        {
            _hw.cpu.Tick(); // For every 3 PPU cycles, the CPU runs one
        }

        _hw.ppu.Tick();
    }
}

void NESemu::Reset()
{
    _hw.Reset(_mapper, _parser.GetMirroringMode());
}

void NESemu::SetControllerState(int controllerNumber, const Input::ControllerState& state)
{
    _hw.input.SetControllerState(controllerNumber, state);
}

uint8_t NESemu::ReadMem(uint16_t address)
{
    return _mapper->ReadMem(address);
}

void NESemu::WriteMem(uint16_t address, uint8_t value)
{
    _mapper->WriteMem(address, value);
}
