#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(SBCTest)
    {
    public:

        TEST_METHOD(SBC)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // No overflow, carry
            rom[codeIndex++] = 0x38; // SEC
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x10; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x05; // Value to subtract

            // No overflow, carry, zero
            rom[codeIndex++] = 0x38; // SEC
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x33; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x33; // Value to subtract

            // No overflow, no carry
            rom[codeIndex++] = 0x38; // SEC
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x00; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x01; // Value to subtract

            // Overflow, carry
            rom[codeIndex++] = 0x38; // SEC
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x80; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x01; // Value to subtract

            // Overflow, no carry
            rom[codeIndex++] = 0x38; // SEC
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x7F; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0xFF; // Value to subtract

            // 16-bit subtraction
            rom[codeIndex++] = 0x38; // SEC
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0xFF; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x34; // Value to subtract
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0xFF; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x12; // Value to subtract

            // 16-bit subtraction, negative result
            rom[codeIndex++] = 0x38; // SEC
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x11; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x12; // Value to subtract
            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x11; // Value to load
            rom[codeIndex++] = 0xE9; // SBC Immediate
            rom[codeIndex++] = 0x11; // Value to subtract

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // No overflow, carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0B, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // No overflow, carry, zero
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // No overflow, no carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // Overflow, carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x7F, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // Overflow, no carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x80, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // 16-bit subtraction
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xCB, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xED, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // 16-bit subtraction, negative result
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);
        }

    };
}