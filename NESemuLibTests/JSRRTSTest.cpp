#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(JSRRTSTest)
    {
    public:

        TEST_METHOD(JSRRTS)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x20; // JSR
            rom[codeIndex++] = 0x34; // Absolute value low-byte
            rom[codeIndex++] = 0x92; // Absolute value high-byte
            rom[0x1234] = 0x60; // RTS

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // JSR
            uint16_t initialProgramCounter = cpu.GetProgramCounter();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(0x9234, (int)cpu.GetProgramCounter() + 1);
            Assert::AreEqual(6, cycles);
            const uint16_t pushedAddress = cpu.PeekStack(0) + (cpu.PeekStack(1) << 8);
            Assert::AreEqual(initialProgramCounter + 3, (int)pushedAddress);

            // RTS
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(initialProgramCounter + 3, (int)cpu.GetProgramCounter());
            Assert::AreEqual(6, cycles);
        }

    };
}