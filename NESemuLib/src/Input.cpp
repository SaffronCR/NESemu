#include "Input.h"

#include "Assert.h"
#include "SizeOfArray.h"

Input::Input()
{
}

Input::~Input()
{
}

uint8_t Input::ReadMem(uint16_t address)
{
    if (address == kController1Address)
    {
        return ReadInput(_4016, _current4016Index);
    }
    else if (address == kController2Address)
    {
        return ReadInput(_4017, _current4017Index);
    }
    else
    {
        OMBAssert(false, "Address %#06x is not part of Input!", address);
        return 0;
    }
}

void Input::WriteMem(uint16_t address, uint8_t value)
{
    OMBAssert(address == kController1Address, "Address %#06x is not part of Input!", address);

    if (_strobe == 0x1 && value == 0x0)
    {
        _current4016Index = 0;
        _current4017Index = 0;
    }
    _strobe = value;
}

void Input::PowerOn()
{
    Reset();
}

void Input::Reset()
{
    for (int i = 0; i < sizeofarray(_4016); ++i)
    {
        _4016[i] = false;
    }
    
    for (int i = 0; i < sizeofarray(_4017); ++i)
    {
        _4017[i] = false;
    }

    _current4016Index = 0;
    _current4017Index = 0;
    _strobe = 0x0;
}

void Input::SetControllerState(int controllerNumber, const ControllerState& state)
{
    bool* curPos;
    if (controllerNumber == 1)
    {
        curPos = _4016;
        _4016[19] = true; // Signature
    }
    else if (controllerNumber == 2)
    {
        curPos = _4017;
        _4017[18] = true; // Signature
    }
    else if (controllerNumber == 3)
    {
        curPos = _4016 + 8;
    }
    else if (controllerNumber == 4)
    {
        curPos = _4017 + 8;
    }
    else
    {
        OMBAssert(false, "Unsupported controller number %d", controllerNumber);
        return;
    }

    *curPos++ = state.A;
    *curPos++ = state.B;
    *curPos++ = state.Select;
    *curPos++ = state.Start;
    *curPos++ = state.Up;
    *curPos++ = state.Down;
    *curPos++ = state.Left;
    *curPos++ = state.Right;
}

uint8_t Input::ReadInput(bool* inputValues, uint8_t& currentIndex)
{
    // From http://wiki.nesdev.com/w/index.php/Standard_controller
    //  In the NES and Famicom, the top three (or five) bits are not driven, and so retain the bits of the previous byte on the bus.
    //  Usually this is the most significant byte of the address of the controller port—0x40. Paperboy relies on this behavior and 
    //  requires that reads from the controller ports return exactly $40 or $41 as appropriate.
    const uint8_t lastBusValue = 0x40;

    // "While S (strobe) is high, the shift registers in the controllers are continuously reloaded from the button states, 
    // and reading $4016/$4017 will keep returning the current state of the first button (A)"
    if (_strobe == 0x1)
    {
        return (uint8_t)inputValues[0] | lastBusValue;
    }

    if (currentIndex >= kInputArraySize)
    {
        OMBAssert(false, "input read out of bounds!");
        currentIndex = 0;
    }

    uint8_t value = inputValues[currentIndex];
    ++currentIndex;
    return value | lastBusValue;
}
