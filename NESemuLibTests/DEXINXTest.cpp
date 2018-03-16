#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(DEXINXTest)
    {
    public:

        TEST_METHOD(DEXINX)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0xA2; // LDX Immediate
            rom[codeIndex++] = 0x00; // Value to load
            rom[codeIndex++] = 0xE8; // INX
            rom[codeIndex++] = 0xCA; // DEX
            rom[codeIndex++] = 0xCA; // DEX
            rom[codeIndex++] = 0xE8; // INX

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, cpu.GetX());
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x01, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetX());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, cpu.GetX());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
        }

    };
}