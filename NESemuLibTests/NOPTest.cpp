#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(NOPTest)
    {
    public:

        TEST_METHOD(NOP)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0xEA; // NOP

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0xFF);
            cpu.SetFlag(CPU::Flag::Carry, true);
            const uint8_t prevStatus = cpu.GetStatus();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(0xFF, (int)cpu.GetAccumulator());
            Assert::AreEqual(prevStatus, cpu.GetStatus());
            Assert::AreEqual(2, cycles);
        }

    };
}