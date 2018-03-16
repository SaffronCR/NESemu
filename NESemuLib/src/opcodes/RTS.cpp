#include "CPU.h"

int CPU::RTS(AddressingMode mode)
{
    _programCounter = Pop() + (Pop() << 8);
    return 6;
}
