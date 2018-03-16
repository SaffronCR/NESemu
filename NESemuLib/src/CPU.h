#pragma once

#include <stdint.h>

class MemoryMapper;

class CPU
{
public:
    static const int kStackSize = 256;
    static const uint16_t kStackAddressStart = 0x100;
    static const uint16_t kNMIVectorAddressL = 0xFFFA;
    static const uint16_t kNMIVectorAddressH = kNMIVectorAddressL + 1;
    static const uint16_t kResetVectorAddressL = 0xFFFC;
    static const uint16_t kResetVectorAddressH = kResetVectorAddressL + 1;
    static const uint16_t kInterruptBreakVectorAddressL = 0xFFFE;
    static const uint16_t kInterruptBreakVectorAddressH = kInterruptBreakVectorAddressL + 1;

    enum Flag
    {
        Carry = 0,          // C
        Zero,               // Z
        InterruptDisable,   // I
        DecimalMode,        // D
        Break,              // B
        Unused,             // 1
        Overflow,           // V
        Sign                // S (1 for Negative)
    };

    enum AddressingMode
    {
        Implied,
        Accumulator,
        Relative,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        Indirect,
        IndirectX,
        IndirectY
    };

    void PowerOn();
    void Reset(MemoryMapper* memoryMapper);

    void Tick();
    int ExecuteNextInstruction(); // Returns number of CPU cycles taken
    void ExecuteNMI();

    // Internal or for testing purposes (hence the publicness) Don't use these without a good reason
    uint16_t GetProgramCounter() const { return _programCounter; }
    void SetProgramCounter(uint16_t value) { _programCounter = value; }
    uint8_t GetStatus() const { return _status; }
    uint8_t GetAccumulator() const { return _accumulator; }
    uint8_t GetX() const { return _x; }
    uint8_t GetY() const { return _y; }
    void SetAccumulator(uint8_t value);
    void SetX(uint8_t value) { _x = value; }
    void SetY(uint8_t value) { _y = value; }
    void SetFlag(Flag flag, bool value);
    bool GetFlag(Flag flag) const;
    uint8_t PeekStack(int index);
    uint8_t GetStackPointer() const { return _stackPointer; }
    void SetStackPointer(uint8_t value) { _stackPointer = value; }
    void EnableOpcodeInfoPrinting(bool value) { _enableOpcodeInfoPrinting = value; }

private:
    uint16_t _programCounter;
    uint8_t _stackPointer;
    uint8_t _status;
    uint8_t _accumulator;
    uint8_t _x;
    uint8_t _y;

    MemoryMapper* _memoryMapper = nullptr;
    int _ticksUntilNextInstruction = 0;
    uint16_t _lastReadAddress;

    bool _enableOpcodeInfoPrinting = false;

    uint8_t GetValueWithMode(AddressingMode mode, int& cycles);
    void SetValueWithMode(AddressingMode mode, uint8_t value, int& cycles);
    void SaveShiftOperationResult(AddressingMode mode, uint8_t result);
    void Push(uint8_t value);
    uint8_t Pop();
    void PrintOpcodeInfo(uint8_t opcode, const char* opcodeName, AddressingMode addressingMode);

    uint8_t GetLowByte(uint16_t value) const;
    uint8_t GetHighByte(uint16_t value) const;
    bool IsValueNegative(uint8_t value) const;

    int LDA(AddressingMode mode);
    int LDX(AddressingMode mode);
    int LDY(AddressingMode mode);
    
    int STA(AddressingMode mode);
    int STX(AddressingMode mode);
    int STY(AddressingMode mode);

    int ADC(AddressingMode mode);
    int SBC(AddressingMode mode);

    int CLC(AddressingMode mode);
    int SEC(AddressingMode mode);
    int CLI(AddressingMode mode);
    int SEI(AddressingMode mode);
    int CLV(AddressingMode mode);
    int CLD(AddressingMode mode);
    int SED(AddressingMode mode);

    int DEX(AddressingMode mode);
    int INX(AddressingMode mode);
    int DEY(AddressingMode mode);
    int INY(AddressingMode mode);
    int DECINC(AddressingMode mode, int delta);

    int JMP(AddressingMode mode);
    int JSR(AddressingMode mode);
    int RTS(AddressingMode mode);
    int RTI(AddressingMode mode);

    int CMP(AddressingMode mode, uint8_t registerValue); // Used for all compare instructions

    int BIT(AddressingMode mode);
    int AND(AddressingMode mode);
    int ORA(AddressingMode mode);
    int EOR(AddressingMode mode);

    int ASL(AddressingMode mode);
    int LSR(AddressingMode mode);
    int ROL(AddressingMode mode);
    int ROR(AddressingMode mode);

    int BXX(AddressingMode mode, bool condition); // Used for all branch instructions

    int TXX(AddressingMode mode, uint8_t from, uint8_t& to); // Used for all transfer instructions
    int PHA(AddressingMode mode);
    int PLA(AddressingMode mode);
    int PHP(AddressingMode mode);
    int PLP(AddressingMode mode);

    int BRK(AddressingMode mode);
    int NOP(AddressingMode mode);
};
