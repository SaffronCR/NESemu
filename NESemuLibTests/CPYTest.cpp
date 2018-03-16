#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(CPYTest)
    {
    public:

        TEST_METHOD(CPY)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Equal values
            rom[codeIndex++] = 0xC0; // CPY Immediate
            rom[codeIndex++] = 0x44; // Value to compare with

            // Compared value is greater than A (and A is positive)
            rom[codeIndex++] = 0xC0; // CPY Immediate
            rom[codeIndex++] = 0x60; // Value to compare with

            // Compared value is lower than A (and A is negative)
            rom[codeIndex++] = 0xC0; // CPY Immediate
            rom[codeIndex++] = 0x05; // Value to compare with

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // Equal values
            cpu.SetY(0x44);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // Compared value is greater than Y (and Y is positive)
            cpu.SetY(0x44);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // Compared value is lower than Y (and Y is negative)
            cpu.SetY(0x90);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);
        }

    };
}