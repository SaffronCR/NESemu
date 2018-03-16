#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(LDATest)
    {
    public:

        TEST_METHOD(LDA)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;

            // Immediate
            int codeIndex = 0;
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x33; // #$33
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x00; // #0
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xFF; // #-128

            // Zero Page
            rom[codeIndex++] = 0xA5; // LDA (Zero Page)
            rom[codeIndex++] = 0x44; // $44
            rom[codeIndex++] = 0xA5; // LDA (Zero Page)
            rom[codeIndex++] = 0x44; // $44
            rom[codeIndex++] = 0xA5; // LDA (Zero Page)
            rom[codeIndex++] = 0x44; // $44

            // Zero Page, X
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x04; // #$4
            rom[codeIndex++] = 0xB5; // LDA (Zero Page, X)
            rom[codeIndex++] = 0x40; // $40

            // Absolute
            rom[codeIndex++] = 0xAD; // LDA (Absolute)
            rom[codeIndex++] = 0x34; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            // Absolute, X (cross boundary, +1 cycle)
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0xFA; // #$FA
            rom[codeIndex++] = 0xBD; // LDA (Absolute, X)
            rom[codeIndex++] = 0x3A; // Low part of address
            rom[codeIndex++] = 0x11; // High part of address

            // Absolute, Y (don't cross boundary)
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x01; // #$01
            rom[codeIndex++] = 0xB9; // LDA (Absolute, Y)
            rom[codeIndex++] = 0x33; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            // Indirect, X
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x04; // #$04
            rom[codeIndex++] = 0xA1; // LDA (Indirect, X)
            rom[codeIndex++] = 0x20; // $20

            // Indirect, Y (don't cross boundary)
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x05; // #$05
            rom[codeIndex++] = 0xB1; // LDA (Indirect, Y)
            rom[codeIndex++] = 0x20; // $20

            // Indirect, Y (cross boundary, +1 cycle)
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0xFF; // #$FF
            rom[codeIndex++] = 0xB1; // LDA (Indirect, Y)
            rom[codeIndex++] = 0x20; // $20

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            ////////////
            // Immediate
            ////////////

            // Positive number
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x33, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            // Zero
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            // Negative number
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(2, cycles);

            ////////////
            // Zero Page
            ////////////

            // Positive number
            emu.WriteMem(0x44, 0x12);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x12, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            // Zero
            emu.WriteMem(0x44, 0x0);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            // Negative number
            emu.WriteMem(0x44, 0xFF);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(3, cycles);

            ///////////////
            // Zero Page, X
            ///////////////
            emu.WriteMem(0x44, 0x66);
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x66, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);

            ///////////
            // Absolute
            ///////////
            emu.WriteMem(0x1234, 0x77);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x77, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);

            //////////////
            // Absolute, X
            //////////////
            emu.WriteMem(0x1234, 0xFC);
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFC, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(5, cycles);

            //////////////
            // Absolute, Y
            //////////////
            emu.WriteMem(0x1234, 0x22);
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x22, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(4, cycles);

            //////////////
            // Indirect, X
            //////////////
            emu.WriteMem(0x24, 0x11);
            emu.WriteMem(0x25, 0x1F);
            emu.WriteMem(0x1F11, 0x63);
            cpu.ExecuteNextInstruction(); // LDX
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x63, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(6, cycles);

            //////////////
            // Indirect, Y
            //////////////
            emu.WriteMem(0x20, 0x33);
            emu.WriteMem(0x21, 0x00);
            emu.WriteMem(0x38, 0xFF);
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(5, cycles);

            ///////////////////////////////
            // Indirect, Y (cross boundary)
            ///////////////////////////////
            emu.WriteMem(0x20, 0x33);
            emu.WriteMem(0x21, 0x00);
            emu.WriteMem(0x132, 0x01);
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x01, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(6, cycles);
        }

    };
}