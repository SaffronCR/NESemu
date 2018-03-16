#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(RORTest)
    {
    public:

        TEST_METHOD(ROR)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // ROR Accumulator, change to negative
            rom[codeIndex++] = 0x6A; // ROR Accumulator

            // ROR Zero Page, change to zero
            rom[codeIndex++] = 0x66; // ROR Zero Page
            rom[codeIndex++] = 0x33; // Zero page value

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0x00);
            cpu.SetFlag(CPU::Flag::Carry, true);
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Carry));
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Carry));
            Assert::AreEqual((uint8_t)0x80, cpu.GetAccumulator());
            Assert::AreEqual(2, cycles);

            emu.WriteMem(0x33, 0x01);
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