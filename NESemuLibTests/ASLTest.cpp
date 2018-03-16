#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(ASLTest)
    {
    public:

        TEST_METHOD(ASL)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // ASL Accumulator, change to negative, no carry
            rom[codeIndex++] = 0x0A; // ASL Accumulator

            // ASL Zero Page, change to zero/positive, carry
            rom[codeIndex++] = 0x06; // ASL Zero Page
            rom[codeIndex++] = 0x33; // Zero page value

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0x7F);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Carry));
            Assert::AreEqual((uint8_t)0xFE, cpu.GetAccumulator());
            Assert::AreEqual(2, cycles);

            emu.WriteMem(0x33, 0x80);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Carry));
            Assert::AreEqual((uint8_t)0x0, emu.ReadMem(0x33));
            Assert::AreEqual(5, cycles);
        }

    };
}