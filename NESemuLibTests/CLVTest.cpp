#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(CLVTest)
    {
    public:

        TEST_METHOD(CLV)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Force an overflow to happen
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x7F; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0x01; // Value to add

            // Clear overflow
            rom[codeIndex++] = 0xB8; // CLV

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // Force an overflow to happen
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Overflow));

            // Clear overflow
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(2, cycles);
        }

    };
}