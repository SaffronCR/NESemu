#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(ADCTest)
    {
    public:

        TEST_METHOD(ADC)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[PRGROM::kMaxPRGROMSize];
            rom[CPU::kResetVectorAddressL - PRGROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - PRGROM::kStartAddress] = PRGROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // No overflow, no carry
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xB8; // CLV
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x33; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0x20; // Value to add

            // No overflow, carry
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xB8; // CLV
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xFF; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0x05; // Value to add

            // No overflow, carry, zero
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xB8; // CLV
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xFF; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0x01; // Value to add

            // Overflow, no carry
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xB8; // CLV
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x7F; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0x01; // Value to add

            // Overflow, carry
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xB8; // CLV
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0x80; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0xFF; // Value to add

            // Verify carry adds to a second ADC
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xB8; // CLV
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xFF; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0x02; // Value to add
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0x05; // Value to add

            emu.Load(rom, PRGROM::kMaxPRGROMSize);

            // No overflow, no carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x53, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // No overflow, carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x04, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // No overflow, carry, zero
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x0, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // Overflow, no carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x80, cpu.GetAccumulator());
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // Overflow, carry
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x7F, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);

            // Verify carry adds to a second ADC
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cycles = cpu.ExecuteNextInstruction();
            cycles += cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x7, cpu.GetAccumulator());
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Overflow));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Zero));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(4, cycles);
        }

    };
}