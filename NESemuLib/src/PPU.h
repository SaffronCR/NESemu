#pragma once

#include "MemoryHandler.h"
#include "MirroringMode.h"

class CHRROM;
class CPU;
class MemoryMapper;

class PPU : public MemoryHandler
{
public:
    static const int kHorizontalResolution = 256;
    static const int kVerticalResolution = 240;
    
    static const int kSpriteSize = 4;
    static const int kSpriteCount = 64;
    static const int kOAMSize = kSpriteCount * kSpriteSize;
    static const int kNametableSize = 0x400;
    static const int kTileWidth = 8;
    static const int kTileHeight = 8;

    static const int kDMARegisterAddress = 0x4014;
    static const int kPPUCtrlAddress = 0x2000;
    static const int kPPUStatusAddress = 0x2002;
    static const int kOAMAddress = 0x2003;
    static const int kOAMData = 0x2004;
    static const int kPPUAddrAddress = 0x2006;
    static const int kPPUDataAddress = 0x2007;
    static const int kNametable0StartAddress = 0x2000;
    static const int kNametable1StartAddress = 0x2400;
    static const int kNametable2StartAddress = 0x2800;
    static const int kNametable3StartAddress = 0x2C00;

    PPU();
    virtual ~PPU();

    // Important: These refer to the memory as seen by the rest of the system (i.e. CPU)
    // This is different from the internally-addressed space
    virtual uint8_t ReadMem(uint16_t address);
    virtual void WriteMem(uint16_t address, uint8_t value);

    virtual void PowerOn();
    virtual void Reset(MemoryMapper* memoryMapper, CPU* cpu, CHRROM* chrRom, MirroringMode mirroringMode);

    void Tick();

    uint32_t* GetFrameBuffer();
    void SetWaitToShowFrameBuffer(bool value) { _waitToShowFrameBuffer = value; }
    bool IsWaitingToShowFrameBuffer() const { return _waitingToShowFrameBuffer; }

    // Internal and for testing purposes
    void SetMirroringMode(MirroringMode mirroringMode);
    int GetSpriteHeight() const;

private:
    enum PPUCtrlFlags
    {
        // 0 and 1 represent Name Table Address
        AddressIncrement = 2,
        SpritePatternTableAddress = 3,
        BkgPatternTableAddress = 4,
        SpriteSize = 5,
        Unused = 6,
        ExecuteNMIOnVBlank = 7
    };

    enum PPUMaskFlags
    {
        ColourMode = 0,
        BkgClipping = 1,
        SpriteClipping = 2,
        BkgVisibility = 3,
        SpriteVisibility = 4,
        // 5 to 7 represent colour mask
    };

    enum PPUStatusFlags
    {
        SpriteOverflow = 5,
        Sprite0Hit = 6,
        VBlank = 7
    };

    enum SpriteLayer
    {
        Front,
        Behind
    };

    static const int kVRAMSize = 2048;
    static const int kPaletteArraySize = 1 + 4*3 + 1*3 + 4*3; // 1 for Bkg colour + 4 Background palettes with extra data between them + 4 Sprite palettes
    static const int kMaxSpritesInSecondaryOAM = 8;
    static const int kSecondaryOAMSize = kMaxSpritesInSecondaryOAM * kSpriteSize;
    
    static const int kPatternTable0Address = 0x0000;
    static const int kPatternTable1Address = 0x1000;
    static const int kMirrorStartAddress = 0x3000;
    static const int kPaletteStartAddress = 0x3F00;
    static const int kUniversalBkgColourAddress = 0x3F00;
    static const int kShadowVRAMStartAddress = 0x4000;

    static const int kLeftClippingPixelCount = 8;
    static const int kCyclesPerScanline = 341;
    static const int kPreRenderScanline = -1;
    static const int kVisibleScanlinesStart = 0;
    static const int kVisibleScanlinesEnd = 239;
    static const int kVerticalBlankingScanlinesStart = 241;
    static const int kVerticalBlankingScanlinesEnd = 260;
    static const uint8_t kTransparentPixelColour = 255;

    // Each sprite is represented by 4 bytes. These are the offsets for each part of its info.
    static const int kSpriteYPosOffset = 0;
    static const int kSpriteTileIndexOffset = 1;
    static const int kSpriteAttributesOffset = 2;
    static const int kSpriteXPosOffset = 3;

    // Palette from http://wiki.nesdev.com/w/index.php/File:Savtool-swatches.png
    const uint32_t kOutputPalette[64] = {
        0x636363FF,
        0x002D69FF,
        0x131F7FFF,
        0x3C137CFF,
        0x600B62FF,
        0x730A37FF,
        0x710F07FF,
        0x5A1A00FF,
        0x342800FF,
        0x0B3400FF,
        0x003C00FF,
        0x003D10FF,
        0x003840FF,
        0x000000FF,
        0x000000FF,
        0x000000FF,

        0xAEAEAEFF,
        0x0F63B3FF,
        0x4051D0FF,
        0x7841CCFF,
        0xA736A9FF,
        0xC03470FF,
        0xBD3C30FF,
        0x9f4A00FF,
        0x6D5C00FF,
        0x366D00FF,
        0x077704FF,
        0x00793DFF,
        0x00727DFF,
        0x000000FF,
        0x000000FF,
        0x000000FF,

        0xFEFEFFFF,
        0x5DB3FFFF,
        0x8FA1FFFF,
        0xC890FFFF,
        0xF785FAFF,
        0xFF83C0FF,
        0xFF8B7FFF,
        0xEF9A49FF,
        0xBDAC2CFF,
        0x85BC2FFF,
        0x55C753FF,
        0x3CC98CFF,
        0x3EC2CDFF,
        0x4E4E4EFF,
        0x000000FF,
        0x000000FF,

        0xFEFEFFFF,
        0xBCDFFFFF,
        0xD1D8FFFF,
        0xE8D1FFFF,
        0xFBCDFDFF,
        0xFFCCE5FF,
        0xFFCFCAFF,
        0xF8D5B4FF,
        0xE4DCA8FF,
        0xCCE3A9FF,
        0xB9E8B8FF,
        0xAEE8D0FF,
        0xAFE5EAFF,
        0xB6B6B6FF,
        0x000000FF,
        0x000000FF,
    };

    // Registers
    uint8_t _ppuCtrl;
    uint8_t _ppuMask;
    uint8_t _ppuStatus;
    uint8_t _oamAddr;

    /*
        http://wiki.nesdev.com/w/index.php/PPU_scrolling

        The 15 bit registers t and v are composed this way during rendering:

        yyy NN YYYYY XXXXX
        ||| || ||||| +++++-- coarse X scroll
        ||| || +++++-------- coarse Y scroll
        ||| ++-------------- nametable select
        +++----------------- fine Y scroll
    */
    uint16_t _v;
    uint16_t _t;
    uint8_t _x;
    bool _w; // false: Next write to 0x2005 and 0x2006 sets upper byte, true: Next write sets lower byte

    uint8_t _readBuffer;
    int _currentScanline = -1;
    int _scanlineCycleIndex;
    bool _isOddFrame;
    bool _waitingToShowFrameBuffer = false;
    bool _waitToShowFrameBuffer = false; // If true, the PPU won't start rendering the next frame until GetFrameBuffer() is called

    uint8_t _vram[kVRAMSize];
    uint8_t _oam[kOAMSize];
    uint8_t _secondaryOAM[kSecondaryOAMSize];
    uint8_t _palettes[kPaletteArraySize];
    uint32_t _frameBuffer[PPU::kHorizontalResolution * PPU::kVerticalResolution];

    MemoryMapper* _memoryMapper;
    CHRROM* _chrRom;
    CPU* _cpu;
    MirroringMode _mirroringMode;
    int _secondaryOAMSpriteCount;
    bool _isSpriteZeroInSecondaryOAM;

    void ResetInternal(bool fullReset);
    uint8_t* GetNametableMem(uint16_t address);
    uint8_t ReadPPUMem(uint16_t address);
    void WritePPUMem(uint16_t address, uint8_t value);
    uint16_t ConvertToRealVRAMAddress(uint16_t address) const;
    uint8_t GetAddressIncrement() const;
    uint8_t ConvertAddressToPaletteIndex(uint16_t address) const;
    void RenderPixel(int x, int y);
    void SetPixelInFrameBuffer(int x, int y, uint8_t paletteIndex);
    uint16_t GetBkgPatternTableBaseAddress() const;
    uint16_t GetSpritePatternTableBaseAddress(uint8_t& spriteIndexNumber) const;
    void ClearSecondaryOAM();
    void FindSpritesInScanline(int index);

    // These 2 functions return kTransparentPixelColour if no opaque pixel was found
    uint8_t CalculateSpriteColourAt(int x, int y, SpriteLayer& layer, bool& isSprite0);
    uint8_t CalculateBkgColourAt(int x, int y);

    void PreRenderScanlineTick();
    void VisibleScanlineTick();
    void VerticalBlankingStartScanlineTick();
};
