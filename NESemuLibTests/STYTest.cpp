#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(STYTest)
    {
    public:

        TEST_METHOD(STY)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Zero Page
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x33; // #$33
            rom[codeIndex++] = 0x84; // STY (Zero Page)
            rom[codeIndex++] = 0x44; // $44

            // Zero Page, Y
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x66; // #$66
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x04; // #$4
            rom[codeIndex++] = 0x94; // STY (Zero Page, Y)
            rom[codeIndex++] = 0x30; // $30

            // Absolute
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x11; // #$11
            rom[codeIndex++] = 0x8C; // STY (Absolute)
            rom[codeIndex++] = 0x34; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            ////////////
            // Zero Page
            ////////////
            cpu.ExecuteNextInstruction(); // LDA
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x33, emu.ReadMem(0x44));
            Assert::AreEqual(3, cycles);

            ///////////////
            // Zero Page, Y
            ///////////////
            cpu.ExecuteNextInstruction(); // LDY
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x66, emu.ReadMem(0x34));
            Assert::AreEqual(4, cycles);

            ///////////
            // Absolute
            ///////////
            cpu.ExecuteNextInstruction(); // LDA
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x11, emu.ReadMem(0x1234));
            Assert::AreEqual(4, cycles);
        }

    };
}