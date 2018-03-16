#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(ORATest)
    {
    public:

        TEST_METHOD(ORA)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Result is zero, S = 0
            rom[codeIndex++] = 0x09; // ORA Immediate
            rom[codeIndex++] = 0x00; // Value

            // Result is not zero, S = 0
            rom[codeIndex++] = 0x05; // ORA ZeroPage
            rom[codeIndex++] = 0x33; // Zero page value

            // Result is not zero, S = 1
            rom[codeIndex++] = 0x0D; // ORA Absolute
            rom[codeIndex++] = 0x34; // Address low byte
            rom[codeIndex++] = 0x12; // Address high byte

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual((uint8_t)0x0, cpu.GetAccumulator());
            Assert::AreEqual(2, cycles);

            cpu.SetAccumulator(0x66);
            emu.WriteMem(0x33, 0x19);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual((uint8_t)0x7F, cpu.GetAccumulator());
            Assert::AreEqual(3, cycles);

            cpu.SetAccumulator(0x81);
            emu.WriteMem(0x1234, 0xFE);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(4, cycles);
        }

    };
}