#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(JMPTest)
    {
    public:

        TEST_METHOD(JMP)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // 1. JMP Absolute
            rom[codeIndex++] = 0x4C; // JMP Absolute
            rom[codeIndex++] = 0x34; // Absolute value low-byte
            rom[codeIndex++] = 0x12; // Absolute value high-byte
            
            // 2. JMP Indirect
            rom[codeIndex++] = 0x6C; // JMP Indirect
            rom[codeIndex++] = 0x22; // Indirect value low-byte
            rom[codeIndex++] = 0x11; // Indirect value high-byte

            // 3. JMP Indirect, check carry doesn't work
            rom[codeIndex++] = 0x6C; // JMP Indirect
            rom[codeIndex++] = 0xFF; // Indirect value low-byte
            rom[codeIndex++] = 0x10; // Indirect value high-byte

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // 1. JMP Absolute
            uint16_t initialProgramCounter = cpu.GetProgramCounter();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(0x1234, (int)cpu.GetProgramCounter() + 1);
            Assert::AreEqual(3, cycles);

            // 2. JMP Indirect
            cpu.SetProgramCounter(initialProgramCounter + 3);
            emu.WriteMem(0x1122, 0x11);
            emu.WriteMem(0x1123, 0x11);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(0x1111, (int)cpu.GetProgramCounter() + 1);
            Assert::AreEqual(5, cycles);

            // 3. JMP Indirect, check carry doesn't work
            cpu.SetProgramCounter(initialProgramCounter + 6);
            emu.WriteMem(0x10FF, 0x33);
            emu.WriteMem(0x1100, 0x22);
            emu.WriteMem(0x1000, 0x66);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(0x6633, (int)cpu.GetProgramCounter() + 1);
            Assert::AreEqual(5, cycles);
        }

    };
}