#include "CPU.h"

int CPU::RTI(AddressingMode mode)
{
    _status = Pop() & ~(1 << Flag::Break) & ~(1 << Flag::Unused);
    _programCounter = Pop() | (Pop() << 8);

    return 6;
}