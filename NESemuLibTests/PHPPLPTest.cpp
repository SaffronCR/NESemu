#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(PHPPLPTest)
    {
    public:

        TEST_METHOD(PHPPLP)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x08; // PHP
            rom[codeIndex++] = 0x28; // PLP

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.SetAccumulator(0xFF);
            cpu.SetFlag(CPU::Flag::Carry, true);
            const uint8_t prevStatus = cpu.GetStatus();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(prevStatus, cpu.PeekStack(0));
            Assert::AreEqual(3, cycles);

            cpu.SetAccumulator(0x00);
            cpu.SetFlag(CPU::Flag::Carry, false);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(prevStatus, cpu.GetStatus());
            Assert::AreEqual(4, cycles);
        }

    };
}