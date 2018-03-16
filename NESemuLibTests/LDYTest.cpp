#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(LDYTest)
    {
    public:

        TEST_METHOD(LDY)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;

            // Immediate
            int codeIndex = 0;
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x33; // #$33
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x00; // #0
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0xFF; // #-128

            // Zero Page
            rom[codeIndex++] = 0xA4; // LDY (Zero Page)
            rom[codeIndex++] = 0x44; // $44
            rom[codeIndex++] = 0xA4; // LDY (Zero Page)
            rom[codeIndex++] = 0x44; // $44
            rom[codeIndex++] = 0xA4; // LDY (Zero Page)
            rom[codeIndex++] = 0x44; // $44

            // Zero Page, X
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x04; // #$4
            rom[codeIndex++] = 0xB4; // LDY (Zero Page, X)
            rom[codeIndex++] = 0x40; // $40

            // Absolute
            rom[codeIndex++] = 0xAC; // LDY (Absolute)
            rom[codeIndex++] = 0x34; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            // Absolute, X (don't cross boundary)
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x01; // #$01
            rom[codeIndex++] = 0xBC; // LDY (Absolute, X)
            rom[codeIndex++] = 0x33; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            ////////////
            // Immediate
            ////////////

            // Positive number
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x33, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            // Zero
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetY());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            // Negative number
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            ////////////
            // Zero Page
            ////////////

            // Positive number
            emu.WriteMem(0x44, 0x12);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x12, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            // Zero
            emu.WriteMem(0x44, 0x0);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetY());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            // Negative number
            emu.WriteMem(0x44, 0xFF);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            ///////////////
            // Zero Page, X
            ///////////////
            emu.WriteMem(0x44, 0x66);
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x66, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);

            ///////////
            // Absolute
            ///////////
            emu.WriteMem(0x1234, 0x77);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x77, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);

            //////////////
            // Absolute, X
            //////////////
            emu.WriteMem(0x1234, 0x22);
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x22, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);
        }

    };
}