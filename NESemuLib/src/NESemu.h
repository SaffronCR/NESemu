#pragma once

#include "Hardware.h"
#include "INESParser.h"

#include <stdint.h>

class MemoryMapper;

class NESemu
{
public:
	NESemu();
	~NESemu();

	void Load(const char* path);
	void Load(const uint8_t rom[], uint16_t romSize); // For testing purposes
	void Load(const uint8_t rom[], uint16_t romSize, const uint8_t chrRom[], uint16_t chrRomSize); // For testing purposes
	void Update(double delta);
	void Reset();
	void SetControllerState(int controllerNumber, const Input::ControllerState& state);

	CPU* GetCPU() { return &_hw.cpu; }
	PPU* GetPPU() { return &_hw.ppu; }
	APU* GetAPU() { return &_hw.apu; }

	uint8_t ReadMem(uint16_t address);
	void WriteMem(uint16_t address, uint8_t value);

private:
	const int kMasterClockSpeed = 21477272; // Hz

	Hardware _hw;
	INESParser _parser;
	MemoryMapper* _mapper = nullptr;
};
