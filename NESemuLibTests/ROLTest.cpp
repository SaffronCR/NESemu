#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(ROLTest)
    {
    public:

        TEST_METHOD(ROL)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // ROL Accumulator, change to negative, carry shifted to bit 0
            rom[codeIndex++] = 0x2A; // ROL Accumulator

            // ROL Zero Page, change to zero
            rom[codeIndex++] = 0x26; // ROL Zero Page
            rom[codeIndex++] = 0x33; // Zero page value

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0x7F);
            cpu.SetFlag(CPU::Flag::Carry, true);
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Carry));
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Carry));
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(2, cycles);

            emu.WriteMem(0x33, 0x80);
            cpu.SetFlag(CPU::Flag::Carry, false);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Carry));
            Assert::AreEqual((uint8_t)0x0, emu.ReadMem(0x33));
            Assert::AreEqual(5, cycles);
        }

    };
}