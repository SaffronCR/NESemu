#include "PPU.h"

#include "Assert.h"
#include "BitwiseUtils.h"
#include "CHRROM.h"
#include "CPU.h"
#include "LogUtils.h"
#include "MemoryMapper.h"
#include "SizeOfArray.h"

PPU::PPU()
{
}

PPU::~PPU()
{
}

uint8_t PPU::ReadMem(uint16_t address)
{
    if (address == 0x2002)
    {
        // Reset PPU Scroll and Addr write toggle so next write to any of them sets the high byte
        _w = false;

        // Reset VBlank flag
        const uint8_t statusBeforeVBlankReset = _ppuStatus;
        if (_ppuStatus != 0)
        {
            BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::VBlank, false);
        }

        return statusBeforeVBlankReset;
    }
    else if (address == 0x2004)
    {
        return _oam[_oamAddr];
    }
    else if (address == 0x2007)
    {
        const uint16_t ppuAddress = ConvertToRealVRAMAddress(_v);
        _v += GetAddressIncrement();
        const uint8_t prevReadBuffer = _readBuffer;
        _readBuffer = ReadPPUMem(ppuAddress);

        if (ppuAddress < kPaletteStartAddress)
        {
            // Non-palette values are buffered, so it requires 2 reads after address set (via 0x2006) to get the value you expect
            return prevReadBuffer;
        }
        else
        {
            // Palette values are returned immediately (although also cached)
            return _readBuffer;
        }
    }
    else if (address >= 0x2008 && address < 0x4000)
    {
        // Shadow PPU Registers
        return ReadMem(((address - 0x2000) % (0x2008 - 0x2000)) + 0x2000);
    }
    else
    {
        OMBAssert(false, "Unimplemented");
        return 0;
    }
}

void PPU::WriteMem(uint16_t address, uint8_t value)
{
    if (address == 0x2000)
    {
        const bool wasNMIFlagSet = BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::ExecuteNMIOnVBlank);
        _ppuCtrl = value;

        // Set nametable bits of t
        _t = (_t & 0xF3FF) | ((value & 0x3) << 10);

        // "When turning on the NMI flag in bit 7, if the PPU is currently in vertical blank 
        // and the PPUSTATUS ($2002) vblank flag is set, an NMI will be generated immediately"
        if (!wasNMIFlagSet && BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::ExecuteNMIOnVBlank) && BitwiseUtils::IsFlagSet(_ppuStatus, PPUStatusFlags::VBlank))
        {
            _cpu->ExecuteNMI();
        }
    }
    else if (address == 0x2001)
    {
        _ppuMask = value;
    }
    else if (address == 0x2003)
    {
        _oamAddr = value;
    }
    else if (address == 0x2004)
    {
        _oam[_oamAddr++] = value;
    }
    else if (address == 0x2005)
    {
        if (_w)
        {
            // "$2005 second write (w is 1)"
            _t = (_t & 0x8C1F) | ((value & 0x7) << 12) | ((value & 0xF8) << 2);
        }
        else
        {
            // "$2005 first write (w is 0)"
            _t = (_t & 0xFFE0) | (value >> 3);
            _x = value & 0x7;
        }

        _w = !_w;
    }
    else if (address == 0x2006)
    {
        if (_w)
        {
            // "$2006 second write (w is 1)"
            _t = (_t & 0xFF00) | value;
            _v = _t;
        }
        else
        {
            // "$2006 first write (w is 0)"
            _t = (_t & 0x80FF) | ((value & 0x3F) << 8);
        }
        
        _w = !_w;
    }
    else if (address == 0x2007)
    {
        const uint16_t ppuAddress = ConvertToRealVRAMAddress(_v);
        _v += GetAddressIncrement();
        WritePPUMem(ppuAddress, value);
    }
    else if (address >= 0x2008 && address < 0x4000)
    {
        // Shadow PPU Registers
        WriteMem(((address - 0x2000) % (0x2008 - 0x2000)) + 0x2000, value);
    }
    else if (address == kDMARegisterAddress)
    {
        for (int i = 0; i < sizeofarray(_oam); ++i)
        {
            _oam[i] = _memoryMapper->ReadMem((value << 8) + i);
        }
    }
    else
    {
        OMBAssert(false, "Unimplemented");
    }
}

void PPU::PowerOn()
{
    ResetInternal(true);
}

void PPU::Reset(MemoryMapper* memoryMapper, CPU* cpu, CHRROM* chrRom, MirroringMode mirroringMode)
{
    _memoryMapper = memoryMapper;
    _cpu = cpu;
    _chrRom = chrRom;
    _mirroringMode = mirroringMode;

    ResetInternal(false);
}

void PPU::ResetInternal(bool fullReset)
{
    // See http://wiki.nesdev.com/w/index.php/PPU_power_up_state
    _ppuCtrl = 0;
    _ppuMask = 0;
    _w = false;
    _readBuffer = 0;

    _currentScanline = -1;
    _scanlineCycleIndex = -1;
    _isOddFrame = true;
    _waitingToShowFrameBuffer = false;

    if (fullReset)
    {
        _ppuStatus = 0;
        _oamAddr = 0;
        _v = 0;
        _t = 0;
        _x = 0;
        _waitToShowFrameBuffer = false;

        for (int i = 0; i < sizeofarray(_vram); ++i)
        {
            _vram[i] = 0;
        }

        for (int i = 0; i < sizeofarray(_oam); ++i)
        {
            _oam[i] = 0;
        }
    }
}

void PPU::Tick()
{
    // Main reference for this function implementation: http://wiki.nesdev.com/w/index.php/PPU_rendering

    // Exit early if the last finished frame hasn't been shown yet to avoid tearing
    if (_waitingToShowFrameBuffer)
    {
        return;
    }

    // Increment cycle/scanline as appropriate
    ++_scanlineCycleIndex;
    if (_scanlineCycleIndex >= kCyclesPerScanline)
    {
        _scanlineCycleIndex = 0;
        ++_currentScanline;
        if (_currentScanline > kVerticalBlankingScanlinesEnd)
        {
            _currentScanline = kPreRenderScanline;
            _isOddFrame = !_isOddFrame;
        }
    }

    // NOTE: Scanlines not handled here are just idle
    if (_currentScanline == kPreRenderScanline)
    {
        PreRenderScanlineTick();
    }
    else if (_currentScanline >= kVisibleScanlinesStart && _currentScanline <= kVisibleScanlinesEnd)
    {
        VisibleScanlineTick();
    }
    else if (_currentScanline == kVerticalBlankingScanlinesStart)
    {
        VerticalBlankingStartScanlineTick();
    }
}

uint32_t* PPU::GetFrameBuffer()
{
    _waitingToShowFrameBuffer = false;
    return _frameBuffer;
}

void PPU::SetMirroringMode(MirroringMode mirroringMode)
{
    _mirroringMode = mirroringMode;
}

uint16_t PPU::ConvertToRealVRAMAddress(uint16_t address) const
{
    return address % kShadowVRAMStartAddress; // Shadow VRAM
}

uint8_t PPU::GetAddressIncrement() const
{
    if (!BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::AddressIncrement))
    {
        return 1;
    }
    else
    {
        return 32;
    }
}

uint8_t* PPU::GetNametableMem(uint16_t address)
{
    if (address >= kNametable0StartAddress && address < kNametable1StartAddress)
    {
        return &_vram[address - kNametable0StartAddress];
    }
    else if (address >= kNametable1StartAddress && address < kNametable2StartAddress)
    {
        const int offset = address % kNametable1StartAddress;
        if (_mirroringMode == MirroringMode::Vertical)
        {
            return &_vram[kNametableSize + offset];
        }
        else
        {
            return &_vram[offset];
        }
    }
    else if (address >= kNametable2StartAddress && address < kNametable3StartAddress)
    {
        const int offset = address % kNametable2StartAddress;
        if (_mirroringMode == MirroringMode::Vertical)
        {
            return &_vram[offset];
        }
        else
        {
            return &_vram[kNametableSize + offset];
        }
    }
    else if (address >= kNametable3StartAddress && address < kMirrorStartAddress)
    {
        const int offset = address % kNametable3StartAddress;
        return &_vram[kNametableSize + offset];
    }
    else
    {
        OMBAssert(false, "$%04X address is not a nametable address", address);
        return 0;
    }
}

uint8_t PPU::ReadPPUMem(uint16_t address)
{
    if (address < kNametable0StartAddress)
    {
        return _chrRom->ReadMem(address);
    }
    else if (address < kMirrorStartAddress)
    {
        return *GetNametableMem(address);
    }
    else if (address < kPaletteStartAddress)
    {
        return *GetNametableMem(address - 0x1000);
    }
    else if (address < 0x4000)
    {
        const uint8_t index = ConvertAddressToPaletteIndex(address);
        return _palettes[index];
    }
    else
    {
        OMBAssert(false, "Unimplemented");
        return 0;
    }
}

void PPU::WritePPUMem(uint16_t address, uint8_t value)
{
    if (address >= kNametable0StartAddress && address < kMirrorStartAddress)
    {
        *GetNametableMem(address) = value;
    }
    else if (address >= kMirrorStartAddress && address < kPaletteStartAddress)
    {
        *GetNametableMem(address - 0x1000) = value;
    }
    else if (address >= kPaletteStartAddress && address < 0x4000)
    {
        const uint8_t index = ConvertAddressToPaletteIndex(address);
        _palettes[index] = value;
    }
    else
    {
        OMBAssert(false, "PPU memory at $%04X is not writtable", address);
    }
}

uint8_t PPU::ConvertAddressToPaletteIndex(uint16_t address) const
{
    const uint16_t realAddress = ((address - kPaletteStartAddress) % 0x20) + kPaletteStartAddress;
    int index = -1;
    if (realAddress == 0x3F00 || realAddress == 0x3F10)
    {
        index = 0;
    }
    else if (realAddress >= 0x3F01 && realAddress <= 0x3F0F)
    {
        // Background palettes 0-3 + their extra data
        index = realAddress - kPaletteStartAddress;
    }
    else if (realAddress == 0x3F10 || realAddress == 0x3F14 || realAddress == 0x3F18 || realAddress == 0x3F1C)
    {
        index = realAddress - 0x3F10; // Mirrors
    }
    else if (realAddress >= 0x3F11 && realAddress <= 0x3F13)
    {
        // Sprite palette 0
        index = realAddress - 0x3F11 + 16;
    }
    else if (realAddress >= 0x3F15 && realAddress <= 0x3F17)
    {
        // Sprite palette 1
        index = realAddress - 0x3F11 - 1 + 16;
    }
    else if (realAddress >= 0x3F19 && realAddress <= 0x3F1B)
    {
        // Sprite palette 2
        index = realAddress - 0x3F11 - 2 + 16;
    }
    else if (realAddress >= 0x3F1D && realAddress <= 0x3F1F)
    {
        // Sprite palette 3
        index = realAddress - 0x3F11 - 3 + 16;
    }
    else
    {
        OMBAssert(false, "Address out of palette space");
    }

    OMBAssert(index >= 0 && index <= sizeofarray(_palettes), "Index out of bounds!");
    return index;
}

void PPU::RenderPixel(int x, int y)
{
    OMBAssert(x >= 0 && x < kHorizontalResolution && y >= 0 && y < kVerticalResolution, "Pixel is out of bounds!");

    bool isSprite0;
    SpriteLayer spriteLayer;
    const uint8_t spritePixelColour = CalculateSpriteColourAt(x, y, spriteLayer, isSprite0);
    const uint8_t bkgPixelColour = CalculateBkgColourAt(x, y);

    // See "Sprite zero hits" section of http://wiki.nesdev.com/w/index.php/PPU_OAM
    if (isSprite0 &&
        x != 255 &&
        spritePixelColour != kTransparentPixelColour &&
        bkgPixelColour != kTransparentPixelColour)
    {
        BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::Sprite0Hit, true);
    }

    if (spritePixelColour != kTransparentPixelColour &&
        (spriteLayer == SpriteLayer::Front || bkgPixelColour == kTransparentPixelColour))
    {
        SetPixelInFrameBuffer(x, y, spritePixelColour);
    }
    else if (bkgPixelColour != kTransparentPixelColour)
    {
        SetPixelInFrameBuffer(x, y, bkgPixelColour);
    }
    else
    {
        SetPixelInFrameBuffer(x, y, ReadPPUMem(kUniversalBkgColourAddress));
    }
}

void PPU::SetPixelInFrameBuffer(int x, int y, uint8_t paletteIndex)
{
    OMBAssert(paletteIndex < sizeofarray(kOutputPalette), "Palette index is out of bounds!");
    const int pixelIndex = y * kHorizontalResolution + x;
    _frameBuffer[pixelIndex] = kOutputPalette[paletteIndex];
}

uint16_t PPU::GetBkgPatternTableBaseAddress() const
{
    if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::BkgPatternTableAddress))
    {
        return kPatternTable1Address;
    }
    else
    {
        return kPatternTable0Address;
    }
}

uint16_t PPU::GetSpritePatternTableBaseAddress(uint8_t& spriteIndexNumber) const
{
    // 8x16 sprite mode
    if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::SpriteSize))
    {
        if ((spriteIndexNumber & 0x1) == 1)
        {
            spriteIndexNumber &= ~(0x1); // Set last bit to 0, as it represents the pattern table
            return kPatternTable1Address;
        }
        else
        {
            return kPatternTable0Address;
        }
    }
    else
    {
        if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::SpritePatternTableAddress))
        {
            return kPatternTable1Address;
        }
        else
        {
            return kPatternTable0Address;
        }
    }
}

int PPU::GetSpriteHeight() const
{
    return BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::SpriteSize) ? 16 : 8;
}

void PPU::ClearSecondaryOAM()
{
    _secondaryOAMSpriteCount = 0;
    _isSpriteZeroInSecondaryOAM = false;
    for (int i = 0; i < sizeofarray(_secondaryOAM); ++i)
    {
        _secondaryOAM[i] = 0xFF;
    }
}

void PPU::FindSpritesInScanline(int index)
{
    OMBAssert(index > 0, "The first scanline has no sprites!");
    ClearSecondaryOAM();

    const int spriteHeight = GetSpriteHeight();

    for (int i = 0; i < kOAMSize && _secondaryOAMSpriteCount < kMaxSpritesInSecondaryOAM; i += kSpriteSize)
    {
        // Add +1 to spriteY because "Sprite data is delayed by one scanline; 
        // you must subtract 1 from the sprite's Y coordinate before writing it here"
        const uint8_t spriteY = _oam[i + kSpriteYPosOffset] + 1;
        if (spriteY > 0 && index >= spriteY && index < spriteY + spriteHeight)
        {
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 0] = _oam[i + 0];
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 1] = _oam[i + 1];
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 2] = _oam[i + 2];
            _secondaryOAM[_secondaryOAMSpriteCount * kSpriteSize + 3] = _oam[i + 3];
            ++_secondaryOAMSpriteCount;
            if (i == 0)
            {
                _isSpriteZeroInSecondaryOAM = true;
            }
        }
    }
}

uint8_t PPU::CalculateSpriteColourAt(int x, int y, SpriteLayer& layer, bool& isSprite0)
{
    if (BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::SpriteVisibility) &&
        (x >= kLeftClippingPixelCount || BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::SpriteClipping)))
    {
        for (int i = 0; i < _secondaryOAMSpriteCount * kSpriteSize; i += kSpriteSize)
        {
            const uint8_t spriteX = _secondaryOAM[i + kSpriteXPosOffset];
            if (x >= spriteX && x < spriteX + kTileWidth)
            {
                const uint8_t spriteY = _secondaryOAM[i + kSpriteYPosOffset] + 1;
                uint8_t patternIndex = _secondaryOAM[i + kSpriteTileIndexOffset];
                const uint16_t patternTableBaseAddress = GetSpritePatternTableBaseAddress(patternIndex);
                const uint8_t attributes = _secondaryOAM[i + kSpriteAttributesOffset];
                
                const uint8_t paletteNumber = attributes & 0x3;
                const bool flipHorizontally = (attributes & (1 << 6)) != 0;
                const bool flipVertically = (attributes & (1 << 7)) != 0;
                layer = (attributes & (1 << 5)) == 0 ? SpriteLayer::Front : SpriteLayer::Behind;

                uint8_t patternPixelX = x - spriteX;
                if (flipHorizontally)
                {
                    patternPixelX = kTileWidth - 1 - patternPixelX;
                }

                uint8_t patternPixelY = y - spriteY;
                if (flipVertically)
                {
                    patternPixelY = kTileHeight - 1 - patternPixelY;
                }

                if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::SpriteSize))
                {
                    // Normally the second half of the sprite is located 0x10 bytes later
                    // than the first half. But when flipping verticallya 8x16 sprite
                    // the are inverted
                    if ((patternPixelY > 7 && !flipVertically) ||
                        (patternPixelY <= 7 && flipVertically))
                    {
                        patternIndex += 1;
                    }
                    
                    patternPixelY %= 8;
                }
                
                const uint16_t patternBaseAddress = patternTableBaseAddress + (patternIndex << 4) + patternPixelY;
                const bool colourBit0 = (ReadPPUMem(patternBaseAddress) & (0x80 >> patternPixelX)) != 0;
                const bool colourBit1 = (ReadPPUMem(patternBaseAddress + 8) & (0x80 >> patternPixelX)) != 0;
                const uint8_t paletteColour = ((int)colourBit1 << 1) | (int)colourBit0;
                if (paletteColour == 0x0)
                {
                    continue;
                }

                const uint8_t paletteIndex = (1 << 4) | paletteNumber << 2 | paletteColour;
                const uint8_t colourIndex = ReadPPUMem(kPaletteStartAddress + paletteIndex);

                isSprite0 = (i == 0) && _isSpriteZeroInSecondaryOAM;
                return colourIndex;
            }
        }
    }

    // No opaque sprites found at this coordinate
    isSprite0 = false;
    return kTransparentPixelColour; // No opaque sprite pixel found at this position
}

uint8_t PPU::CalculateBkgColourAt(int x, int y)
{
    if (BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgVisibility) &&
        (x >= kLeftClippingPixelCount || BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgClipping)))
    {
        // "Tile and attribute fetching" @ http://wiki.nesdev.com/w/index.php/PPU_scrolling
        const uint16_t nametableAddress = 0x2000 | (_v & 0x0FFF);
        const uint16_t attributeAddress = 0x23C0 | (_v & 0x0C00) | ((_v >> 4) & 0x38) | ((_v >> 2) & 0x07);

        const uint8_t nametableTileX = _v & 0x1F;
        const uint8_t nametableTileY = (_v & 0x3E0) >> 5;
        const uint8_t fineX = (_x + x) % kTileWidth;
        const uint8_t fineY = (_v & 0x7000) >> 12;

        const uint8_t patternIndex = ReadPPUMem(nametableAddress);
        const uint16_t patternTableBaseAddress = GetBkgPatternTableBaseAddress();
        const uint16_t patternBaseAddress = patternTableBaseAddress + (patternIndex << 4) + fineY;
        const bool colourBit0 = (ReadPPUMem(patternBaseAddress) & (0x80 >> fineX)) != 0;
        const bool colourBit1 = (ReadPPUMem(patternBaseAddress + 8) & (0x80 >> fineX)) != 0;
        const uint8_t paletteColour = ((int)colourBit1 << 1) | (int)colourBit0;
        if (paletteColour == 0)
        {
            return kTransparentPixelColour;
        }

        // Each attribute element contains 4x4 tiles, and each subarea is made of 2x2 tiles
        const uint8_t attributeArea = (nametableTileY % 4) / 2 << 1 | ((nametableTileX % 4) / 2); // 0:Top-Left, 1:Top-Right, 2:Bottom-Left, 3:Bottom-Right
        const uint8_t attributeValue = ReadPPUMem(attributeAddress);
        const uint8_t paletteNumber = (attributeValue >> (attributeArea * 2)) & 0x3;

        const uint8_t paletteIndex = paletteNumber << 2 | paletteColour;
        const uint8_t colourIndex = ReadPPUMem(kPaletteStartAddress + paletteIndex);

        return colourIndex;
    }

    return kTransparentPixelColour;
}

void PPU::PreRenderScanlineTick()
{
    const bool isRenderingEnabled =
        BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::SpriteVisibility) ||
        BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgVisibility);

    if (_scanlineCycleIndex == 1)
    {
        BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::SpriteOverflow, false);
        BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::Sprite0Hit, false);
        BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::VBlank, false);
        ClearSecondaryOAM();
    }
    else if (isRenderingEnabled && _scanlineCycleIndex == 257)
    {
        // "If rendering is enabled, the PPU copies all bits related to horizontal position from t to v
        _v = (_v & 0xFBE0) | (_t & ~0xFBE0);
    }
    else if (isRenderingEnabled && _scanlineCycleIndex >= 280 && _scanlineCycleIndex <= 304)
    {
        // "If rendering is enabled, at the end of vblank, shortly after the horizontal bits are copied from 
        // t to v at dot 257, the PPU will repeatedly copy the vertical bits from t to v from dots 280 to 304, 
        // completing the full initialization of v from t
        _v = (_v & 0x841F) | (_t & ~0x841F);
    }
    else if (_isOddFrame && _scanlineCycleIndex + 2 == kCyclesPerScanline)
    {
        // "For odd frames, the cycle at the end of the scanline is skipped"
        ++_scanlineCycleIndex;
    }
}

void PPU::VisibleScanlineTick()
{
    const bool isRenderingEnabled =
        BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::SpriteVisibility) ||
        BitwiseUtils::IsFlagSet(_ppuMask, PPUMaskFlags::BkgVisibility);

    if (_scanlineCycleIndex >= 1 && _scanlineCycleIndex <= kHorizontalResolution)
    {
        if (isRenderingEnabled)
        {
            const int xPlusScroll = _scanlineCycleIndex - 1 + _x;
            const bool gotToNextTile = (xPlusScroll >= 8) && (xPlusScroll % 8 == 0);
            if (gotToNextTile && _scanlineCycleIndex < kHorizontalResolution)
            {
                // "Coarse X increment" @ http://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
                if ((_v & 0x001F) == 31)    // if coarse X == 31
                {
                    _v &= ~0x001F;          // coarse X = 0
                    _v ^= 0x0400;           // switch horizontal nametable
                }
                else
                {
                    _v += 1;                // increment coarse X
                }
            }
            else if (_scanlineCycleIndex == kHorizontalResolution)
            {
                // If rendering is enabled, the PPU increments the vertical position in v.
                // The effective Y scroll coordinate is incremented, which is a complex operation 
                // that will correctly skip the attribute table memory regions, and wrap to the next nametable appropriately
                // See "Y increment" @ http://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
                if ((_v & 0x7000) != 0x7000)            // if fine Y < 7
                {
                    _v += 0x1000;                       // increment fine Y
                }
                else
                {
                    _v &= ~0x7000;                      // fine Y = 0
                    uint16_t y = (_v & 0x03E0) >> 5;    // let y = coarse Y
                    if (y == 29)
                    {
                        y = 0;                          // coarse Y = 0
                        _v ^= 0x0800;                   // switch vertical nametable
                    }
                    else if (y == 31)
                    {
                        y = 0;                          // coarse Y = 0, nametable not switched
                    }
                    else
                    {
                        y += 1;                         // increment coarse Y
                    }
                    _v = (_v & ~0x03E0) | (y << 5);     // put coarse Y back into v
                }
            }
        }

        RenderPixel(_scanlineCycleIndex - 1, _currentScanline);
    }
    else if (_scanlineCycleIndex >= 257 && _scanlineCycleIndex <= 320)
    {
        if (isRenderingEnabled && _scanlineCycleIndex == 257)
        {
            // "If rendering is enabled, the PPU copies all bits related to horizontal position from t to v:
            _v = (_v & 0xFBE0) | (_t & ~0xFBE0);
        }
        else if (isRenderingEnabled && _scanlineCycleIndex == 320)
        {
            // "The tile data for the sprites on the next scanline are fetched here."
            FindSpritesInScanline(_currentScanline + 1);
        }
    }
}

void PPU::VerticalBlankingStartScanlineTick()
{
    if (_scanlineCycleIndex == 1)
    {
        // "The VBlank flag of the PPU is set at tick 1 (the second tick) of scanline 241..."
        BitwiseUtils::SetFlag(_ppuStatus, PPUStatusFlags::VBlank, true);

        // "...where the VBlank NMI also occurs"
        if (BitwiseUtils::IsFlagSet(_ppuCtrl, PPUCtrlFlags::ExecuteNMIOnVBlank))
        {
            _cpu->ExecuteNMI();
        }

        if (_waitToShowFrameBuffer)
        {
            _waitingToShowFrameBuffer = true;
        }
    }
}
