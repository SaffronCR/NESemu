#pragma once

#include "MemoryHandler.h"

//  The NES outputs ~1789772 samples a second.
const long nes_sample_Rate = 1789772;
const long sample_rate = 44100;
const int buf_size = 2048;

class APU : public MemoryHandler
{
public:

	APU();
	virtual ~APU();

	virtual uint8_t ReadMem(uint16_t address);
	virtual void WriteMem(uint16_t address, uint8_t value);
	virtual void PowerOn();
	virtual void Reset();
	virtual void Tick();
	virtual long ReadSamples(uint16_t* buf, long bufSize);

private:

	const uint8_t lengthLookup[32] =
	{
		10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
		12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	};

	// http://wiki.nesdev.com/w/index.php/APU_Pulse
	const uint8_t outputWaveform[4][8] =
	{
		{ 0, 1, 0, 0, 0, 0, 0, 0 },     // 12.5%
		{ 0, 1, 1, 0, 0, 0, 0, 0 },     // 25%
		{ 0, 1, 1, 1, 1, 0, 0, 0 },     // 50%
		{ 1, 0, 0, 1, 1, 1, 1, 1 }      // 25% negated
	};

	struct FrameCounter
	{
		uint8_t mode = 1;
		uint8_t interrupt = 1;
		uint8_t inhibit = 1;
		uint8_t updated = 1;
		uint16_t count = 15;
	};

	struct Envelope
	{
		uint8_t loopHalt = 1;
		uint8_t constVol = 1;
		uint8_t volPeriod = 4;
		uint8_t counter = 4;
		uint8_t divider = 5;
		uint8_t start = 1;
		uint8_t out = 4;
	};

	struct PulseChannel
	{
		Envelope envelope;
		uint16_t timer = 12;
		uint16_t timerPeriod = 11;
		uint8_t counter;
		uint8_t duty = 2;
		uint8_t enabled = 1;
		uint8_t phase = 3;

		uint8_t sweepEnable = 1;
		uint8_t sweepPeriod = 3;
		uint8_t sweepNegate = 1;
		uint8_t sweepShift = 3;
		uint8_t sweepDivider = 4;
		uint16_t sweepTarget = 12;
		uint8_t sweepSilence = 1;
		uint8_t sweepReload = 1;

		void CalcSweep(uint16_t offset = 0)
		{
			if (sweepNegate)
				sweepTarget = timerPeriod - ((timerPeriod >> sweepShift) + offset);
			else
				sweepTarget = timerPeriod + (timerPeriod >> sweepShift);

			if ((timerPeriod < 8) || (sweepTarget > 0x7FF))
				sweepSilence = 1;
			else
				sweepSilence = 0;
		}
	};

	FrameCounter _frameCounter;

	PulseChannel _pulse1;
	PulseChannel _pulse2;

	uint32_t _squareTable[31];

	int _buffer_index = 0;
	uint16_t _outBuf[buf_size];

	int _cycleLimit = (nes_sample_Rate / sample_rate);
	int _cycleCounter = 0;

	uint16_t _averageBuf[(nes_sample_Rate / sample_rate)];
	long _averageSum = 0;

	uint16_t _pulse1Offset = 1;

	void HalfFrame();
	void QuarterFrame();

	void UpdateEnvelope(Envelope* envelope);
	void UpdateCounter(PulseChannel* pulse);
	void UpdateSweep(PulseChannel* pulse, uint16_t offset = 0);
};
