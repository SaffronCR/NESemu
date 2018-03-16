#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(ANDTest)
    {
    public:

        TEST_METHOD(AND)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Result is zero, S = 0
            rom[codeIndex++] = 0x29; // AND Immediate
            rom[codeIndex++] = 0x02; // Value

            // Result is not zero, S = 1
            rom[codeIndex++] = 0x2D; // AND Absolute
            rom[codeIndex++] = 0x34; // Address low byte
            rom[codeIndex++] = 0x12; // Address high byte

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0x01);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual((uint8_t)0x0, cpu.GetAccumulator());
            Assert::AreEqual(2, cycles);

            cpu.SetAccumulator(0x80);
            emu.WriteMem(0x1234, 0xFF);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual((uint8_t)0x80, cpu.GetAccumulator());
            Assert::AreEqual(4, cycles);
        }

    };
}