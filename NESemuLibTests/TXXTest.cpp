#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(TXXTest)
    {
    public:

        TEST_METHOD(TXX)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x9A; // TXS
            rom[codeIndex++] = 0xBA; // TSX
            rom[codeIndex++] = 0xAA; // TAX
            rom[codeIndex++] = 0x8A; // TXA
            rom[codeIndex++] = 0xA8; // TAY
            rom[codeIndex++] = 0x98; // TYA

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // TXS
            cpu.SetX(0x66);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x66, cpu.GetStackPointer());
            Assert::AreEqual(2, cycles);

            // TSX
            cpu.SetStackPointer(0x54);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x54, cpu.GetX());
            Assert::AreEqual(2, cycles);

            // TAX
            cpu.SetAccumulator(0x12);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x12, cpu.GetX());
            Assert::AreEqual(2, cycles);

            // TXA
            cpu.SetFlag(CPU::Flag::Sign, true);
            cpu.SetFlag(CPU::Flag::Zero, true);
            cpu.SetX(0x44);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x44, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);

            // TAY
            cpu.SetAccumulator(0x33);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x33, cpu.GetY());
            Assert::AreEqual(2, cycles);

            // TYA
            cpu.SetFlag(CPU::Flag::Sign, true);
            cpu.SetFlag(CPU::Flag::Zero, false);
            cpu.SetY(0x0);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(2, cycles);
        }

    };
}