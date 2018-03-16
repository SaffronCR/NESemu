#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(BITTest)
    {
    public:

        TEST_METHOD(BIT)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Result is zero, S = 1, V = 0
            rom[codeIndex++] = 0x24; // BIT ZeroPage
            rom[codeIndex++] = 0x44; // Zero Page address

            // Result is not zero, S = 0, V = 1
            rom[codeIndex++] = 0x2C; // BIT Absolute
            rom[codeIndex++] = 0x34; // Address low byte
            rom[codeIndex++] = 0x12; // Address high byte

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0x01);
            emu.WriteMem(0x44, 0x80);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Overflow));
            Assert::AreEqual((uint8_t)0x01, cpu.GetAccumulator());
            Assert::AreEqual(3, cycles);

            cpu.SetAccumulator(0x06);
            emu.WriteMem(0x1234, 0x7F);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Overflow));
            Assert::AreEqual((uint8_t)0x06, cpu.GetAccumulator());
            Assert::AreEqual(4, cycles);
        }

    };
}