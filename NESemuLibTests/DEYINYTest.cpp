#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(DEYINYTest)
    {
    public:

        TEST_METHOD(DEYINY)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0xA0; // LDY Immediate
            rom[codeIndex++] = 0x00; // Value to load
            rom[codeIndex++] = 0xC8; // INY
            rom[codeIndex++] = 0x88; // DEY
            rom[codeIndex++] = 0x88; // DEY
            rom[codeIndex++] = 0xC8; // INY

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, cpu.GetY());
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x01, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetY());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, cpu.GetY());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
        }

    };
}