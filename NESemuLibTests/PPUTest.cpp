#include "CppUnitTest.h"

#include "NESemu.h"
#include "SizeOfArray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(PPUTest)
    {
    public:

        TEST_METHOD(PPUPatternTable)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            uint8_t chrRom[CHRROM::kPageCHRROMSize];
            for (int i = 0; i < CHRROM::kPageCHRROMSize; ++i)
            {
                chrRom[i] = i % 256;
            }

            emu.Load(rom, PRGROM::kMaxPRGROMSize, chrRom, CHRROM::kMaxCHRROMSize);

            // Verify PatternTable in VRAM and its shadows
            for (int i = 0; i < 4; ++i)
            {
                // Read 0x2002 to reset PPUAddr status
                emu.ReadMem(PPU::kPPUStatusAddress);
                
                // Write address to read from
                emu.WriteMem(PPU::kPPUAddrAddress, 0x40 * i);
                emu.WriteMem(PPU::kPPUAddrAddress, 0x00);

                // Discard first read, as it has a cached value
                emu.ReadMem(PPU::kPPUDataAddress);

                // Verify all values
                for (int i = 0; i < CHRROM::kPageCHRROMSize; ++i)
                {
                    Assert::AreEqual(i % 256, (int)emu.ReadMem(PPU::kPPUDataAddress));
                }
            }
        }

        TEST_METHOD(PPUNameTable)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // Read 0x2002 to reset PPUAddr status
            emu.ReadMem(PPU::kPPUStatusAddress);

            emu.GetPPU()->SetMirroringMode(MirroringMode::Vertical);

            // Set address of Nametable 0
            emu.WriteMem(PPU::kPPUAddrAddress, 0x20);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);

            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                // Write values to Nametable 0-2
                emu.WriteMem(PPU::kPPUDataAddress, 0x33);
            }

            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                // Write values to Nametable 1-3
                emu.WriteMem(PPU::kPPUDataAddress, 0x66);
            }

            // Check Nametable 0
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x20);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 0 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x30);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x24);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x34);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x28);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x38);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x2C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3 (Shadow, partial)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x3C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < 0x2FF; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            /////////////////////////////////////
            // SAME BUT WITH HORIZONTAL MIRRORING
            /////////////////////////////////////
            emu.GetPPU()->SetMirroringMode(MirroringMode::Horizontal);

            // Check Nametable 0
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x20);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 0 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x30);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x24);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 1 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x34);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x33, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x28);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 2 (Shadow)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x38);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x2C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < PPU::kNametableSize; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Check Nametable 3 (Shadow, partial)
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x2C);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            emu.ReadMem(PPU::kPPUDataAddress); // Dummy read
            for (int i = 0; i < 0x2FF; ++i)
            {
                Assert::AreEqual(0x66, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }
        }

        TEST_METHOD(PPUPalette)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x3F);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            
            int nextValue = 0x10;

            // Bkg colour
            emu.WriteMem(PPU::kPPUDataAddress, nextValue++);

            // Background palettes + extra data
            for (int i = 0; i < 4*3 + 3*1; ++i)
            {
                emu.WriteMem(PPU::kPPUDataAddress, nextValue++);
            }

            emu.ReadMem(PPU::kPPUDataAddress); // Skip background mirror

            // Sprite palettes
            for (int i = 1; i < 4*4 - 1; ++i)
            {
                if (i % 4 != 0)
                {
                    emu.WriteMem(PPU::kPPUDataAddress, nextValue++);
                }
                else
                {
                    // Do not write mirror data
                    emu.ReadMem(PPU::kPPUDataAddress);
                }
            }

            // Verify written data
            nextValue = 0x10;
            emu.ReadMem(PPU::kPPUStatusAddress);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x3F);
            emu.WriteMem(PPU::kPPUAddrAddress, 0x00);
            
            // Bkg colour
            const int bkgValue = nextValue++;
            Assert::AreEqual(bkgValue, (int)emu.ReadMem(PPU::kPPUDataAddress));
            
            // Background palettes + extra data
            for (int i = 0; i < 4 * 3 + 3 * 1; ++i)
            {
                Assert::AreEqual(nextValue++, (int)emu.ReadMem(PPU::kPPUDataAddress));
            }

            // Bkg colour mirror
            Assert::AreEqual(bkgValue, (int)emu.ReadMem(PPU::kPPUDataAddress));

            // Sprite palettes + mirrors
            for (int i = 1; i < 4*4 - 1; ++i)
            {
                if (i % 4 != 0)
                {
                    // Sprite palettes
                    Assert::AreEqual(nextValue++, (int)emu.ReadMem(PPU::kPPUDataAddress));
                }
                else
                {
                    // Mirrors
                    Assert::AreEqual(0x10 + i/4 * 4, (int)emu.ReadMem(PPU::kPPUDataAddress));
                }
            }
        }

        TEST_METHOD(PPUOAM)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // Write OAM data
            emu.WriteMem(PPU::kOAMAddress, 0x00);
            for (int i = 0; i < PPU::kOAMSize; ++i)
            {
                emu.WriteMem(PPU::kOAMData, i);
            }

            // Verify OAM data
            for (int i = 0; i < PPU::kOAMSize; ++i)
            {
                emu.WriteMem(PPU::kOAMAddress, i); // Reads do not increment OAM address, so set for each read
                Assert::AreEqual(i, (int)emu.ReadMem(PPU::kOAMData));
            }
        }

        TEST_METHOD(PPUDMA)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // Init RAM with values to transfer
            for (int i = 0; i <= 0x0700; i += 0x100)
            {
                for (int j = 0; j < 0x100; ++j)
                {
                    emu.WriteMem(i + j, i / 0x100);
                }
            }

            // Test DMA
            for (int i = 0; i <= 7; ++i)
            {
                emu.WriteMem(PPU::kDMARegisterAddress, i);
                for (int j = 0; j < PPU::kOAMSize; ++j)
                {
                    emu.WriteMem(PPU::kOAMAddress, j); // Reads do not increment OAM address, so set for each read
                    Assert::AreEqual(i, (int)emu.ReadMem(PPU::kOAMData));
                }
            }
        }
    };
}