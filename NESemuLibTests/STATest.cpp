#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(STATest)
    {
    public:

        TEST_METHOD(STA)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Zero Page
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x33; // #$33
            rom[codeIndex++] = 0x85; // STA (Zero Page)
            rom[codeIndex++] = 0x44; // $44

            // Zero Page, X
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x66; // #$66
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x04; // #$4
            rom[codeIndex++] = 0x95; // STA (Zero Page, X)
            rom[codeIndex++] = 0x30; // $30

            // Absolute
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x11; // #$11
            rom[codeIndex++] = 0x8D; // STA (Absolute)
            rom[codeIndex++] = 0x34; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            // Absolute, X
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xBB; // #$BB
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x01; // #$1
            rom[codeIndex++] = 0x9D; // STA (Absolute, X)
            rom[codeIndex++] = 0x21; // Low part of address
            rom[codeIndex++] = 0x11; // High part of address

            // Absolute, Y
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xCC; // #$CC
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x05; // #$5
            rom[codeIndex++] = 0x99; // STA (Absolute, Y)
            rom[codeIndex++] = 0x10; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            // Indirect, X
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xDD; // #$DD
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x05; // #$5
            rom[codeIndex++] = 0x81; // STA (Indirect, X)
            rom[codeIndex++] = 0x33; // Zero page address

            // Indirect, Y
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xEE; // #$EE
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x09; // #$9
            rom[codeIndex++] = 0x91; // STA (Indirect, Y)
            rom[codeIndex++] = 0x44; // Zero page address

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            ////////////
            // Zero Page
            ////////////
            cpu.ExecuteNextInstruction(); // LDA
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x33, emu.ReadMem(0x44));
            Assert::AreEqual(3, cycles);
            
            ///////////////
            // Zero Page, X
            ///////////////
            cpu.ExecuteNextInstruction(); // LDA
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

            //////////////
            // Absolute, X
            //////////////
            cpu.ExecuteNextInstruction(); // LDA
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xBB, emu.ReadMem(0x1122));
            Assert::AreEqual(5, cycles);

            //////////////
            // Absolute, Y
            //////////////
            cpu.ExecuteNextInstruction(); // LDA
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xCC, emu.ReadMem(0x1215));
            Assert::AreEqual(5, cycles);

            //////////////
            // Indirect, X
            //////////////
            emu.WriteMem(0x38, 0x00);
            emu.WriteMem(0x39, 0x10);
            cpu.ExecuteNextInstruction(); // LDA
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xDD, emu.ReadMem(0x1000));
            Assert::AreEqual(6, cycles);

            //////////////
            // Indirect, Y
            //////////////
            emu.WriteMem(0x44, 0x00);
            emu.WriteMem(0x45, 0x15);
            cpu.ExecuteNextInstruction(); // LDA
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xEE, emu.ReadMem(0x1509));
            Assert::AreEqual(6, cycles);
        }

    };
}