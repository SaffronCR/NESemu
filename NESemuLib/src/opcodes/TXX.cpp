#include "CPU.h"

int CPU::TXX(AddressingMode mode, uint8_t from, uint8_t& to)
{
    if (&to == &_accumulator)
    {
        // Use function so flags are properly set
        SetAccumulator(from);
    }
    else
    {
        to = from;
    }
    return 2;
}
