#include "CPU.h"

#include "Assert.h"
#include "BitwiseUtils.h"
#include "LogUtils.h"
#include "MemoryMapper.h"

#include <map>

void CPU::PowerOn()
{
    // http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    _programCounter = 0xFFFF;
    _stackPointer = 0;
    _status = 0x34;
    _accumulator = 0;
    _x = 0;
    _y = 0;
}

void CPU::Reset(MemoryMapper* memoryMapper)
{
    _memoryMapper = memoryMapper;

    // Load program start address in program counter
    _programCounter = _memoryMapper->ReadMem(kResetVectorAddressL) + (_memoryMapper->ReadMem(kResetVectorAddressH) << 8) - 1;
    _ticksUntilNextInstruction = 0;
    
    // "After reset S was decremented by 3 (but nothing was written to the stack)" from http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    _stackPointer -= 0x3;
    
    // https://stackoverflow.com/questions/16913423/why-is-the-initial-state-of-the-interrupt-flag-of-the-6502-a-1
    SetFlag(Flag::InterruptDisable, true);
}

void CPU::Tick()
{
    --_ticksUntilNextInstruction;
    if (_ticksUntilNextInstruction <= 0)
    {
        _ticksUntilNextInstruction = ExecuteNextInstruction() - 1;
    }
}

int CPU::ExecuteNextInstruction()
{
    // Fetch next opcode
    const uint8_t opcode = _memoryMapper->ReadMem(++_programCounter);

    switch (opcode)
    {
        // LDA
        case 0xA9: PrintOpcodeInfo(opcode, "LDA", AddressingMode::Immediate); return LDA(AddressingMode::Immediate);
        case 0xA5: PrintOpcodeInfo(opcode, "LDA", AddressingMode::ZeroPage); return LDA(AddressingMode::ZeroPage);
        case 0xB5: PrintOpcodeInfo(opcode, "LDA", AddressingMode::ZeroPageX); return LDA(AddressingMode::ZeroPageX);
        case 0xAD: PrintOpcodeInfo(opcode, "LDA", AddressingMode::Absolute); return LDA(AddressingMode::Absolute);
        case 0xBD: PrintOpcodeInfo(opcode, "LDA", AddressingMode::AbsoluteX); return LDA(AddressingMode::AbsoluteX);
        case 0xB9: PrintOpcodeInfo(opcode, "LDA", AddressingMode::AbsoluteY); return LDA(AddressingMode::AbsoluteY);
        case 0xA1: PrintOpcodeInfo(opcode, "LDA", AddressingMode::IndirectX); return LDA(AddressingMode::IndirectX);
        case 0xB1: PrintOpcodeInfo(opcode, "LDA", AddressingMode::IndirectY); return LDA(AddressingMode::IndirectY);

        // LDX
        case 0xA2: PrintOpcodeInfo(opcode, "LDX", AddressingMode::Immediate); return LDX(AddressingMode::Immediate);
        case 0xA6: PrintOpcodeInfo(opcode, "LDX", AddressingMode::ZeroPage); return LDX(AddressingMode::ZeroPage);
        case 0xB6: PrintOpcodeInfo(opcode, "LDX", AddressingMode::ZeroPageY); return LDX(AddressingMode::ZeroPageY);
        case 0xAE: PrintOpcodeInfo(opcode, "LDX", AddressingMode::Absolute); return LDX(AddressingMode::Absolute);
        case 0xBE: PrintOpcodeInfo(opcode, "LDX", AddressingMode::AbsoluteY); return LDX(AddressingMode::AbsoluteY);

        // LDY
        case 0xA0: PrintOpcodeInfo(opcode, "LDY", AddressingMode::Immediate); return LDY(AddressingMode::Immediate);
        case 0xA4: PrintOpcodeInfo(opcode, "LDY", AddressingMode::ZeroPage); return LDY(AddressingMode::ZeroPage);
        case 0xB4: PrintOpcodeInfo(opcode, "LDY", AddressingMode::ZeroPageX); return LDY(AddressingMode::ZeroPageX);
        case 0xAC: PrintOpcodeInfo(opcode, "LDY", AddressingMode::Absolute); return LDY(AddressingMode::Absolute);
        case 0xBC: PrintOpcodeInfo(opcode, "LDY", AddressingMode::AbsoluteX); return LDY(AddressingMode::AbsoluteX);

        // STA
        case 0x85: PrintOpcodeInfo(opcode, "STA", AddressingMode::ZeroPage); return STA(AddressingMode::ZeroPage);
        case 0x95: PrintOpcodeInfo(opcode, "STA", AddressingMode::ZeroPageX); return STA(AddressingMode::ZeroPageX);
        case 0x8D: PrintOpcodeInfo(opcode, "STA", AddressingMode::Absolute); return STA(AddressingMode::Absolute);
        case 0x9D: PrintOpcodeInfo(opcode, "STA", AddressingMode::AbsoluteX); return STA(AddressingMode::AbsoluteX);
        case 0x99: PrintOpcodeInfo(opcode, "STA", AddressingMode::AbsoluteY); return STA(AddressingMode::AbsoluteY);
        case 0x81: PrintOpcodeInfo(opcode, "STA", AddressingMode::IndirectX); return STA(AddressingMode::IndirectX);
        case 0x91: PrintOpcodeInfo(opcode, "STA", AddressingMode::IndirectY); return STA(AddressingMode::IndirectY);

        // STX
        case 0x86: PrintOpcodeInfo(opcode, "STX", AddressingMode::ZeroPage); return STX(AddressingMode::ZeroPage);
        case 0x96: PrintOpcodeInfo(opcode, "STX", AddressingMode::ZeroPageY); return STX(AddressingMode::ZeroPageY);
        case 0x8E: PrintOpcodeInfo(opcode, "STX", AddressingMode::Absolute); return STX(AddressingMode::Absolute);

        // STY
        case 0x84: PrintOpcodeInfo(opcode, "STY", AddressingMode::ZeroPage); return STY(AddressingMode::ZeroPage);
        case 0x94: PrintOpcodeInfo(opcode, "STY", AddressingMode::ZeroPageX); return STY(AddressingMode::ZeroPageX);
        case 0x8C: PrintOpcodeInfo(opcode, "STY", AddressingMode::Absolute); return STY(AddressingMode::Absolute);

        // ADC
        case 0x69: PrintOpcodeInfo(opcode, "ADC", AddressingMode::Immediate); return ADC(AddressingMode::Immediate);
        case 0x65: PrintOpcodeInfo(opcode, "ADC", AddressingMode::ZeroPage); return ADC(AddressingMode::ZeroPage);
        case 0x75: PrintOpcodeInfo(opcode, "ADC", AddressingMode::ZeroPageX); return ADC(AddressingMode::ZeroPageX);
        case 0x6D: PrintOpcodeInfo(opcode, "ADC", AddressingMode::Absolute); return ADC(AddressingMode::Absolute);
        case 0x7D: PrintOpcodeInfo(opcode, "ADC", AddressingMode::AbsoluteX); return ADC(AddressingMode::AbsoluteX);
        case 0x79: PrintOpcodeInfo(opcode, "ADC", AddressingMode::AbsoluteY); return ADC(AddressingMode::AbsoluteY);
        case 0x61: PrintOpcodeInfo(opcode, "ADC", AddressingMode::IndirectX); return ADC(AddressingMode::IndirectX);
        case 0x71: PrintOpcodeInfo(opcode, "ADC", AddressingMode::IndirectY); return ADC(AddressingMode::IndirectY);

        // SBC
        case 0xE9: PrintOpcodeInfo(opcode, "SBC", AddressingMode::Immediate); return SBC(AddressingMode::Immediate);
        case 0xE5: PrintOpcodeInfo(opcode, "SBC", AddressingMode::ZeroPage); return SBC(AddressingMode::ZeroPage);
        case 0xF5: PrintOpcodeInfo(opcode, "SBC", AddressingMode::ZeroPageX); return SBC(AddressingMode::ZeroPageX);
        case 0xED: PrintOpcodeInfo(opcode, "SBC", AddressingMode::Absolute); return SBC(AddressingMode::Absolute);
        case 0xFD: PrintOpcodeInfo(opcode, "SBC", AddressingMode::AbsoluteX); return SBC(AddressingMode::AbsoluteX);
        case 0xF9: PrintOpcodeInfo(opcode, "SBC", AddressingMode::AbsoluteY); return SBC(AddressingMode::AbsoluteY);
        case 0xE1: PrintOpcodeInfo(opcode, "SBC", AddressingMode::IndirectX); return SBC(AddressingMode::IndirectX);
        case 0xF1: PrintOpcodeInfo(opcode, "SBC", AddressingMode::IndirectY); return SBC(AddressingMode::IndirectY);

        // CPU Flags
        case 0x18: PrintOpcodeInfo(opcode, "CLC", AddressingMode::Implied); return CLC(AddressingMode::Implied);
        case 0x38: PrintOpcodeInfo(opcode, "SEC", AddressingMode::Implied); return SEC(AddressingMode::Implied);
        case 0x58: PrintOpcodeInfo(opcode, "CLI", AddressingMode::Implied); return CLI(AddressingMode::Implied);
        case 0x78: PrintOpcodeInfo(opcode, "SEI", AddressingMode::Implied); return SEI(AddressingMode::Implied);
        case 0xB8: PrintOpcodeInfo(opcode, "CLV", AddressingMode::Implied); return CLV(AddressingMode::Implied);
        case 0xD8: PrintOpcodeInfo(opcode, "CLD", AddressingMode::Implied); return CLD(AddressingMode::Implied);
        case 0xF8: PrintOpcodeInfo(opcode, "SED", AddressingMode::Implied); return SED(AddressingMode::Implied);

        // Increment/Decrement
        case 0xCA: PrintOpcodeInfo(opcode, "DEX", AddressingMode::Implied); return DEX(AddressingMode::Implied);
        case 0xE8: PrintOpcodeInfo(opcode, "INX", AddressingMode::Implied); return INX(AddressingMode::Implied);
        case 0x88: PrintOpcodeInfo(opcode, "DEY", AddressingMode::Implied); return DEY(AddressingMode::Implied);
        case 0xC8: PrintOpcodeInfo(opcode, "INY", AddressingMode::Implied); return INY(AddressingMode::Implied);
        case 0xC6: PrintOpcodeInfo(opcode, "DEC", AddressingMode::ZeroPage); return DECINC(AddressingMode::ZeroPage, -1);
        case 0xD6: PrintOpcodeInfo(opcode, "DEC", AddressingMode::ZeroPageX); return DECINC(AddressingMode::ZeroPageX, -1);
        case 0xCE: PrintOpcodeInfo(opcode, "DEC", AddressingMode::Absolute); return DECINC(AddressingMode::Absolute, -1);
        case 0xDE: PrintOpcodeInfo(opcode, "DEC", AddressingMode::AbsoluteX); return DECINC(AddressingMode::AbsoluteX, -1);
        case 0xE6: PrintOpcodeInfo(opcode, "INC", AddressingMode::ZeroPage); return DECINC(AddressingMode::ZeroPage, +1);
        case 0xF6: PrintOpcodeInfo(opcode, "INC", AddressingMode::ZeroPageX); return DECINC(AddressingMode::ZeroPageX, +1);
        case 0xEE: PrintOpcodeInfo(opcode, "INC", AddressingMode::Absolute); return DECINC(AddressingMode::Absolute, +1);
        case 0xFE: PrintOpcodeInfo(opcode, "INC", AddressingMode::AbsoluteX); return DECINC(AddressingMode::AbsoluteX, +1);

        // Routine instructions
        case 0x4C: PrintOpcodeInfo(opcode, "JMP", AddressingMode::Absolute); return JMP(AddressingMode::Absolute);
        case 0x6C: PrintOpcodeInfo(opcode, "JMP", AddressingMode::Indirect); return JMP(AddressingMode::Indirect);
        case 0x20: PrintOpcodeInfo(opcode, "JSR", AddressingMode::Absolute); return JSR(AddressingMode::Absolute);
        case 0x60: PrintOpcodeInfo(opcode, "RTS", AddressingMode::Implied); return RTS(AddressingMode::Implied);
        case 0x40: PrintOpcodeInfo(opcode, "RTI", AddressingMode::Implied); return RTI(AddressingMode::Implied);

        // Compare instructions
        case 0xC9: PrintOpcodeInfo(opcode, "CMP", AddressingMode::Immediate); return CMP(AddressingMode::Immediate, _accumulator);
        case 0xC5: PrintOpcodeInfo(opcode, "CMP", AddressingMode::ZeroPage); return CMP(AddressingMode::ZeroPage, _accumulator);
        case 0xD5: PrintOpcodeInfo(opcode, "CMP", AddressingMode::ZeroPageX); return CMP(AddressingMode::ZeroPageX, _accumulator);
        case 0xCD: PrintOpcodeInfo(opcode, "CMP", AddressingMode::Absolute); return CMP(AddressingMode::Absolute, _accumulator);
        case 0xDD: PrintOpcodeInfo(opcode, "CMP", AddressingMode::AbsoluteX); return CMP(AddressingMode::AbsoluteX, _accumulator);
        case 0xD9: PrintOpcodeInfo(opcode, "CMP", AddressingMode::AbsoluteY); return CMP(AddressingMode::AbsoluteY, _accumulator);
        case 0xC1: PrintOpcodeInfo(opcode, "CMP", AddressingMode::IndirectX); return CMP(AddressingMode::IndirectX, _accumulator);
        case 0xD1: PrintOpcodeInfo(opcode, "CMP", AddressingMode::IndirectY); return CMP(AddressingMode::IndirectY, _accumulator);
        case 0xE0: PrintOpcodeInfo(opcode, "CPX", AddressingMode::Immediate); return CMP(AddressingMode::Immediate, _x);
        case 0xE4: PrintOpcodeInfo(opcode, "CPX", AddressingMode::ZeroPage); return CMP(AddressingMode::ZeroPage, _x);
        case 0xEC: PrintOpcodeInfo(opcode, "CPX", AddressingMode::Absolute); return CMP(AddressingMode::Absolute, _x);
        case 0xC0: PrintOpcodeInfo(opcode, "CPY", AddressingMode::Immediate); return CMP(AddressingMode::Immediate, _y);
        case 0xC4: PrintOpcodeInfo(opcode, "CPY", AddressingMode::ZeroPage); return CMP(AddressingMode::ZeroPage, _y);
        case 0xCC: PrintOpcodeInfo(opcode, "CPY", AddressingMode::Absolute); return CMP(AddressingMode::Absolute, _y);

        // Logic instructions
        case 0x24: PrintOpcodeInfo(opcode, "BIT", AddressingMode::ZeroPage); return BIT(AddressingMode::ZeroPage);
        case 0x2C: PrintOpcodeInfo(opcode, "BIT", AddressingMode::Absolute); return BIT(AddressingMode::Absolute);
        case 0x29: PrintOpcodeInfo(opcode, "AND", AddressingMode::Immediate); return AND(AddressingMode::Immediate);
        case 0x25: PrintOpcodeInfo(opcode, "AND", AddressingMode::ZeroPage); return AND(AddressingMode::ZeroPage);
        case 0x35: PrintOpcodeInfo(opcode, "AND", AddressingMode::ZeroPageX); return AND(AddressingMode::ZeroPageX);
        case 0x2D: PrintOpcodeInfo(opcode, "AND", AddressingMode::Absolute); return AND(AddressingMode::Absolute);
        case 0x3D: PrintOpcodeInfo(opcode, "AND", AddressingMode::AbsoluteX); return AND(AddressingMode::AbsoluteX);
        case 0x39: PrintOpcodeInfo(opcode, "AND", AddressingMode::AbsoluteY); return AND(AddressingMode::AbsoluteY);
        case 0x21: PrintOpcodeInfo(opcode, "AND", AddressingMode::IndirectX); return AND(AddressingMode::IndirectX);
        case 0x31: PrintOpcodeInfo(opcode, "AND", AddressingMode::IndirectX); return AND(AddressingMode::IndirectX);
        case 0x09: PrintOpcodeInfo(opcode, "ORA", AddressingMode::Immediate); return ORA(AddressingMode::Immediate);
        case 0x05: PrintOpcodeInfo(opcode, "ORA", AddressingMode::ZeroPage); return ORA(AddressingMode::ZeroPage);
        case 0x15: PrintOpcodeInfo(opcode, "ORA", AddressingMode::ZeroPageX); return ORA(AddressingMode::ZeroPageX);
        case 0x0D: PrintOpcodeInfo(opcode, "ORA", AddressingMode::Absolute); return ORA(AddressingMode::Absolute);
        case 0x1D: PrintOpcodeInfo(opcode, "ORA", AddressingMode::AbsoluteX); return ORA(AddressingMode::AbsoluteX);
        case 0x19: PrintOpcodeInfo(opcode, "ORA", AddressingMode::AbsoluteY); return ORA(AddressingMode::AbsoluteY);
        case 0x01: PrintOpcodeInfo(opcode, "ORA", AddressingMode::IndirectX); return ORA(AddressingMode::IndirectX);
        case 0x11: PrintOpcodeInfo(opcode, "ORA", AddressingMode::IndirectX); return ORA(AddressingMode::IndirectX);
        case 0x49: PrintOpcodeInfo(opcode, "EOR", AddressingMode::Immediate); return EOR(AddressingMode::Immediate);
        case 0x45: PrintOpcodeInfo(opcode, "EOR", AddressingMode::ZeroPage); return EOR(AddressingMode::ZeroPage);
        case 0x55: PrintOpcodeInfo(opcode, "EOR", AddressingMode::ZeroPageX); return EOR(AddressingMode::ZeroPageX);
        case 0x4D: PrintOpcodeInfo(opcode, "EOR", AddressingMode::Absolute); return EOR(AddressingMode::Absolute);
        case 0x5D: PrintOpcodeInfo(opcode, "EOR", AddressingMode::AbsoluteX); return EOR(AddressingMode::AbsoluteX);
        case 0x59: PrintOpcodeInfo(opcode, "EOR", AddressingMode::AbsoluteY); return EOR(AddressingMode::AbsoluteY);
        case 0x41: PrintOpcodeInfo(opcode, "EOR", AddressingMode::IndirectX); return EOR(AddressingMode::IndirectX);
        case 0x51: PrintOpcodeInfo(opcode, "EOR", AddressingMode::IndirectX); return EOR(AddressingMode::IndirectX);

        // Shift instructions
        case 0x0A: PrintOpcodeInfo(opcode, "ASL", AddressingMode::Accumulator); return ASL(AddressingMode::Accumulator);
        case 0x06: PrintOpcodeInfo(opcode, "ASL", AddressingMode::ZeroPage); return ASL(AddressingMode::ZeroPage);
        case 0x16: PrintOpcodeInfo(opcode, "ASL", AddressingMode::ZeroPageX); return ASL(AddressingMode::ZeroPageX);
        case 0x0E: PrintOpcodeInfo(opcode, "ASL", AddressingMode::Absolute); return ASL(AddressingMode::Absolute);
        case 0x1E: PrintOpcodeInfo(opcode, "ASL", AddressingMode::AbsoluteX); return ASL(AddressingMode::AbsoluteX);
        case 0x4A: PrintOpcodeInfo(opcode, "LSR", AddressingMode::Accumulator); return LSR(AddressingMode::Accumulator);
        case 0x46: PrintOpcodeInfo(opcode, "LSR", AddressingMode::ZeroPage); return LSR(AddressingMode::ZeroPage);
        case 0x56: PrintOpcodeInfo(opcode, "LSR", AddressingMode::ZeroPageX); return LSR(AddressingMode::ZeroPageX);
        case 0x4E: PrintOpcodeInfo(opcode, "LSR", AddressingMode::Absolute); return LSR(AddressingMode::Absolute);
        case 0x5E: PrintOpcodeInfo(opcode, "LSR", AddressingMode::AbsoluteX); return LSR(AddressingMode::AbsoluteX);
        case 0x2A: PrintOpcodeInfo(opcode, "ROL", AddressingMode::Accumulator); return ROL(AddressingMode::Accumulator);
        case 0x26: PrintOpcodeInfo(opcode, "ROL", AddressingMode::ZeroPage); return ROL(AddressingMode::ZeroPage);
        case 0x36: PrintOpcodeInfo(opcode, "ROL", AddressingMode::ZeroPageX); return ROL(AddressingMode::ZeroPageX);
        case 0x2E: PrintOpcodeInfo(opcode, "ROL", AddressingMode::Absolute); return ROL(AddressingMode::Absolute);
        case 0x3E: PrintOpcodeInfo(opcode, "ROL", AddressingMode::AbsoluteX); return ROL(AddressingMode::AbsoluteX);
        case 0x6A: PrintOpcodeInfo(opcode, "ROR", AddressingMode::Accumulator); return ROR(AddressingMode::Accumulator);
        case 0x66: PrintOpcodeInfo(opcode, "ROR", AddressingMode::ZeroPage); return ROR(AddressingMode::ZeroPage);
        case 0x76: PrintOpcodeInfo(opcode, "ROR", AddressingMode::ZeroPageX); return ROR(AddressingMode::ZeroPageX);
        case 0x6E: PrintOpcodeInfo(opcode, "ROR", AddressingMode::Absolute); return ROR(AddressingMode::Absolute);
        case 0x7E: PrintOpcodeInfo(opcode, "ROR", AddressingMode::AbsoluteX); return ROR(AddressingMode::AbsoluteX);

        // Branch-on-condition instructions
        case 0xF0: PrintOpcodeInfo(opcode, "BEQ", AddressingMode::Relative); return BXX(AddressingMode::Relative, GetFlag(Flag::Zero));
        case 0xD0: PrintOpcodeInfo(opcode, "BNE", AddressingMode::Relative); return BXX(AddressingMode::Relative, !GetFlag(Flag::Zero));
        case 0xB0: PrintOpcodeInfo(opcode, "BCS", AddressingMode::Relative); return BXX(AddressingMode::Relative, GetFlag(Flag::Carry));
        case 0x90: PrintOpcodeInfo(opcode, "BCC", AddressingMode::Relative); return BXX(AddressingMode::Relative, !GetFlag(Flag::Carry));
        case 0x70: PrintOpcodeInfo(opcode, "BVS", AddressingMode::Relative); return BXX(AddressingMode::Relative, GetFlag(Flag::Overflow));
        case 0x50: PrintOpcodeInfo(opcode, "BVC", AddressingMode::Relative); return BXX(AddressingMode::Relative, !GetFlag(Flag::Overflow));
        case 0x30: PrintOpcodeInfo(opcode, "BMI", AddressingMode::Relative); return BXX(AddressingMode::Relative, GetFlag(Flag::Sign));
        case 0x10: PrintOpcodeInfo(opcode, "BPL", AddressingMode::Relative); return BXX(AddressingMode::Relative, !GetFlag(Flag::Sign));

        // Transfer instructions
        case 0x9A: PrintOpcodeInfo(opcode, "TXS", AddressingMode::Implied); return TXX(AddressingMode::Implied, _x, _stackPointer);
        case 0xBA: PrintOpcodeInfo(opcode, "TSX", AddressingMode::Implied); return TXX(AddressingMode::Implied, _stackPointer, _x);
        case 0xAA: PrintOpcodeInfo(opcode, "TAX", AddressingMode::Implied); return TXX(AddressingMode::Implied, _accumulator, _x);
        case 0x8A: PrintOpcodeInfo(opcode, "TXA", AddressingMode::Implied); return TXX(AddressingMode::Implied, _x, _accumulator);
        case 0xA8: PrintOpcodeInfo(opcode, "TAY", AddressingMode::Implied); return TXX(AddressingMode::Implied, _accumulator, _y);
        case 0x98: PrintOpcodeInfo(opcode, "TYA", AddressingMode::Implied); return TXX(AddressingMode::Implied, _y, _accumulator);
        case 0x48: PrintOpcodeInfo(opcode, "PHA", AddressingMode::Implied); return PHA(AddressingMode::Implied);
        case 0x68: PrintOpcodeInfo(opcode, "PLA", AddressingMode::Implied); return PLA(AddressingMode::Implied);
        case 0x08: PrintOpcodeInfo(opcode, "PHP", AddressingMode::Implied); return PHP(AddressingMode::Implied);
        case 0x28: PrintOpcodeInfo(opcode, "PLP", AddressingMode::Implied); return PLP(AddressingMode::Implied);

        // Other
        case 0x00: PrintOpcodeInfo(opcode, "BRK", AddressingMode::Implied); return BRK(AddressingMode::Implied);
        case 0xEA: PrintOpcodeInfo(opcode, "NOP", AddressingMode::Implied); return NOP(AddressingMode::Implied);

        default: Log::Debug("Unknown opcode: $%02X", opcode); return 0;
    };
}

void CPU::ExecuteNMI()
{
    const uint8_t statusPlusBFlag = _status | (1 << Flag::Break) | (1 << Flag::Unused);
    Push(GetHighByte(_programCounter));
    Push(GetLowByte(_programCounter));
    Push(statusPlusBFlag);
    SetFlag(Flag::InterruptDisable, true);
    _programCounter = _memoryMapper->ReadMem(kNMIVectorAddressL) +
        (_memoryMapper->ReadMem(kNMIVectorAddressH) << 8) - 1;
}

void CPU::SetAccumulator(uint8_t value)
{
    _accumulator = value;
    SetFlag(Flag::Sign, IsValueNegative(_accumulator));
    SetFlag(Flag::Zero, _accumulator == 0);
}

void CPU::SetFlag(Flag flag, bool value)
{
    if (flag == Flag::Unused || flag == Flag::Break)
    {
        OMBAssert(false, "Can't set flag %d", flag);
        return;
    }

    BitwiseUtils::SetFlag(_status, flag, value);
}

bool CPU::GetFlag(Flag flag) const
{
    if (flag == Flag::Unused)
    {
        OMBAssert(false, "Unused flag only exists in the stack!");
        return true;
    }
    else if (flag == Flag::Break)
    {
        OMBAssert(false, "Break flag only exists in the stack!");
        return false;
    }
    else
    {
        return BitwiseUtils::IsFlagSet(_status, flag);
    }
}

uint8_t CPU::PeekStack(int index)
{
    return _memoryMapper->ReadMem(kStackAddressStart + _stackPointer + 1 + index);
}

uint8_t CPU::GetValueWithMode(AddressingMode mode, int& cycles)
{
    uint8_t value = 0;
    switch (mode)
    {
        case AddressingMode::Accumulator:
        {
            value = _accumulator;
            cycles += 0; // Not sure about this one. Only used for shift and rotate instructions.
            break;
        }
        case AddressingMode::Immediate:
        {
            value = _memoryMapper->ReadMem(++_programCounter);
            cycles += 1;
            break;
        }
        case AddressingMode::ZeroPage:
        {
            _lastReadAddress = _memoryMapper->ReadMem(++_programCounter);
            value = _memoryMapper->ReadMem(_lastReadAddress);
            cycles += 2;
            break;
        }
        case AddressingMode::ZeroPageX:
        case AddressingMode::ZeroPageY:
        {
            const uint8_t regValue = (mode == AddressingMode::ZeroPageX) ? _x : _y;
            _lastReadAddress = _memoryMapper->ReadMem(++_programCounter) + regValue;
            value = _memoryMapper->ReadMem(_lastReadAddress);
            cycles += 3;
            break;
        }
        case AddressingMode::Absolute:
        {
            _lastReadAddress = _memoryMapper->ReadMem(++_programCounter) + (_memoryMapper->ReadMem(++_programCounter) << 8);
            value = _memoryMapper->ReadMem(_lastReadAddress);
            cycles += 3;
            break;
        }
        case AddressingMode::AbsoluteX:
        case AddressingMode::AbsoluteY:
        {
            const uint8_t regValue = (mode == AddressingMode::AbsoluteX) ? _x : _y;
            const uint8_t loByte = _memoryMapper->ReadMem(++_programCounter);
            const uint16_t loBytePlusReg = loByte + regValue;
            const uint8_t hiByte = _memoryMapper->ReadMem(++_programCounter);
            _lastReadAddress = loBytePlusReg + (hiByte << 8);
            value = _memoryMapper->ReadMem(_lastReadAddress);
            cycles += 3;
            if (loBytePlusReg != (uint8_t)loBytePlusReg)
            {
                // If page boundary crossed when adding low byte and register, it takes 1 more cycle due to carry
                cycles += 1;
            }
            break;
        }
        case AddressingMode::IndirectX:
        {
            const uint16_t address = _memoryMapper->ReadMem(++_programCounter) + _x;
            _lastReadAddress = _memoryMapper->ReadMem(address) + (_memoryMapper->ReadMem(address + 1) << 8);
            value = _memoryMapper->ReadMem(_lastReadAddress);
            cycles += 5;
            break;
        }
        case AddressingMode::IndirectY:
        {
            const uint8_t address = _memoryMapper->ReadMem(++_programCounter);
            const uint8_t loByte = _memoryMapper->ReadMem(address);
            const uint16_t loBytePlusReg = loByte + _y;
            const uint8_t hiByte = _memoryMapper->ReadMem(address + 1);
            _lastReadAddress = loBytePlusReg + (hiByte << 8);
            value = _memoryMapper->ReadMem(_lastReadAddress);
            cycles += 4;
            if (loBytePlusReg != (uint8_t)loBytePlusReg)
            {
                cycles += 1;
            }
            break;
        }
        default:
        {
            OMBAssert(false, "Can't get value with addressingMode %d", mode);
        }
    }

    return value;
}

void CPU::SetValueWithMode(AddressingMode mode, uint8_t value, int& cycles)
{
    switch (mode)
    {
        case AddressingMode::ZeroPage:
        {
            const uint16_t address = _memoryMapper->ReadMem(++_programCounter);
            _memoryMapper->WriteMem(address, value);
            cycles += 2;
            break;
        }
        case AddressingMode::ZeroPageX:
        case AddressingMode::ZeroPageY:
        {
            const uint8_t regValue = (mode == AddressingMode::ZeroPageX) ? _x : _y;
            const uint16_t address = _memoryMapper->ReadMem(++_programCounter) + regValue;
            _memoryMapper->WriteMem(address, value);
            cycles += 3;
            break;
        }
        case AddressingMode::Absolute:
        {
            const uint16_t address = _memoryMapper->ReadMem(++_programCounter) + (_memoryMapper->ReadMem(++_programCounter) << 8);
            _memoryMapper->WriteMem(address, value);
            cycles += 3;
            break;
        }
        case AddressingMode::AbsoluteX:
        case AddressingMode::AbsoluteY:
        {
            const uint8_t regValue = (mode == AddressingMode::AbsoluteX) ? _x : _y;
            const uint8_t loByte = _memoryMapper->ReadMem(++_programCounter);
            const uint16_t loBytePlusReg = loByte + regValue;
            const uint8_t hiByte = _memoryMapper->ReadMem(++_programCounter);
            const uint16_t address = loBytePlusReg + (hiByte << 8);
            _memoryMapper->WriteMem(address, value);
            cycles += 4;
            break;
        }
        case AddressingMode::IndirectX:
        {
            const uint16_t address = _memoryMapper->ReadMem(++_programCounter) + _x;
            const uint16_t finalAddress = _memoryMapper->ReadMem(address) + (_memoryMapper->ReadMem(address + 1) << 8);
            _memoryMapper->WriteMem(finalAddress, value);
            cycles += 5;
            break;
        }
        case AddressingMode::IndirectY:
        {
            const uint8_t address = _memoryMapper->ReadMem(++_programCounter);
            const uint8_t loByte = _memoryMapper->ReadMem(address);
            const uint16_t loBytePlusReg = loByte + _y;
            const uint8_t hiByte = _memoryMapper->ReadMem(address + 1);
            const uint16_t finalAddress = loBytePlusReg + (hiByte << 8);
            _memoryMapper->WriteMem(finalAddress, value);
            cycles += 5;
            break;
        }
        default:
        {
            OMBAssert(false, "Can't set value with addressingMode %d", mode);
        }
    }
}

void CPU::SaveShiftOperationResult(AddressingMode mode, uint8_t result)
{
    if (mode == AddressingMode::Accumulator)
    {
        SetAccumulator(result);
    }
    else
    {
        // These 2 flags are set within SetAccumulator() too, so no need to do it within the previous condition
        SetFlag(Flag::Sign, IsValueNegative(result));
        SetFlag(Flag::Zero, result == 0);

        _memoryMapper->WriteMem(_lastReadAddress, result);
    }
}

void CPU::Push(uint8_t value)
{
    _memoryMapper->WriteMem(kStackAddressStart + _stackPointer, value);
    OMBAssert(_stackPointer > 0, "Stack overflow!");
    --_stackPointer;
}

uint8_t CPU::Pop()
{
    OMBAssert(_stackPointer < 0xFF, "Stack overflow!");
    ++_stackPointer;
    return _memoryMapper->ReadMem(kStackAddressStart + _stackPointer);
}

uint8_t CPU::GetLowByte(uint16_t value) const
{
    return 0x00FF & value;
}

uint8_t CPU::GetHighByte(uint16_t value) const
{
    return value >> 8;
}

bool CPU::IsValueNegative(uint8_t value) const
{
    return (value >> 7) > 0;
}

void CPU::PrintOpcodeInfo(uint8_t opcode, const char* opcodeName, AddressingMode addressingMode)
{
#if _DEBUG
    if (!_enableOpcodeInfoPrinting)
    {
        return;
    }

    const char* addressingModeStr = nullptr;
    switch (addressingMode)
    {
        case AddressingMode::Implied:
            addressingModeStr = "Implied";
            break;
        case AddressingMode::Accumulator: 
            addressingModeStr = "Accumulator"; 
            break;
        case AddressingMode::Relative: 
            addressingModeStr = "Relative"; 
            break;
        case AddressingMode::Immediate: 
            addressingModeStr = "Immediate"; 
            break;
        case AddressingMode::ZeroPage: 
            addressingModeStr = "ZeroPage"; 
            break;
        case AddressingMode::ZeroPageX: 
            addressingModeStr = "ZeroPageX"; 
            break;
        case AddressingMode::ZeroPageY: 
            addressingModeStr = "ZeroPageY"; 
            break;
        case AddressingMode::Absolute: 
            addressingModeStr = "Absolute"; 
            break;
        case AddressingMode::AbsoluteX: 
            addressingModeStr = "AbsoluteX"; 
            break;
        case AddressingMode::AbsoluteY: 
            addressingModeStr = "AbsoluteY"; 
            break;
        case AddressingMode::Indirect: 
            addressingModeStr = "Indirect"; 
            break;
        case AddressingMode::IndirectX: 
            addressingModeStr = "IndirectX"; 
            break;
        case AddressingMode::IndirectY: 
            addressingModeStr = "IndirectY"; 
            break;
    };
    Log::Debug("Executing opcode: %s ($%02X, %s)", opcodeName, opcode, addressingModeStr);
#endif
}
