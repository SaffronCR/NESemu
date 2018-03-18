#include "APU.h"

#include "Assert.h"
#include "LogUtils.h"
#include "NESemu.h"

APU::APU()
{
	for (int i = 0; i < 31; i++)
	{
		_squareTable[i] = (uint32_t)((95.52 / (8128.0 / (double)i + 100.0)) * 0xFFFFFFFFUL);
	}
}

APU::~APU()
{
}

uint8_t APU::ReadMem(uint16_t address)
{
	// Status.
	if (address == 0x4015)
	{
		// TODO
		return 0;
	}

	return 0;
}

void APU::WriteMem(uint16_t address, uint8_t value)
{
	// TODO

	// Info from http://wiki.nesdev.com/w/index.php/APU#Registers
	switch (address)
	{
		// Pulse 1 (Envelope, sweep, timer, length counter).
	case 0x4000:
		_pulse1.duty = value >> 6;
		_pulse1.envelope.loopHalt = value & 0x20;
		_pulse1.envelope.constVol = value & 0x10;
		_pulse1.envelope.volPeriod = value & 0xF;
		break;
	case 0x4001:
		_pulse1.sweepEnable = value & 0x80;
		_pulse1.sweepPeriod = (value >> 4) & 0x07;
		_pulse1.sweepNegate = value & 0x08;
		_pulse1.sweepShift = value & 0x07;
		_pulse1.sweepReload = 1;
		break;
	case 0x4002:
		_pulse1.timerPeriod = (_pulse1.timerPeriod & 0x0700) | value;
		_pulse1.CalcSweep(_pulse1Offset);
		break;
	case 0x4003:
		_pulse1.timerPeriod = (_pulse1.timerPeriod & 0x00FF) | ((value & 0x07) << 8);
		_pulse1.CalcSweep(_pulse1Offset);
		if (_pulse1.enabled)
		{
			_pulse1.counter = lengthLookup[value >> 3];
		}
		_pulse1.phase = 0;
		_pulse1.envelope.start = 1;
		break;

		// Pulse 2 (Envelope, sweep, timer, length counter).
	case 0x4004:
		_pulse2.duty = value >> 6;
		_pulse2.envelope.loopHalt = value & 0x20;
		_pulse2.envelope.constVol = value & 0x10;
		_pulse2.envelope.volPeriod = value & 0xF;
		break;
	case 0x4005:
		_pulse2.sweepEnable = value & 0x80;
		_pulse2.sweepPeriod = (value >> 4) & 0x07;
		_pulse2.sweepNegate = value & 0x08;
		_pulse2.sweepShift = value & 0x07;
		_pulse2.sweepReload = 1;
		break;
	case 0x4006:
		_pulse2.timerPeriod = (_pulse2.timerPeriod & 0x0700) | value;
		_pulse2.CalcSweep();
		break;
	case 0x4007:
		_pulse2.timerPeriod = (_pulse2.timerPeriod & 0x00FF) | ((value & 0x07) << 8);
		_pulse2.CalcSweep();
		if (_pulse2.enabled)
		{
			_pulse2.counter = lengthLookup[value >> 3];
		}
		_pulse2.phase = 0;
		_pulse2.envelope.start = 1;
		break;

		// Triangle (Linear counter, timer, length counter).
	case 0x4008:
		break;
	case 0x400A:
		break;
	case 0x400B:
		break;

		// Noise (Volume, period, length counter).
	case 0x400C:
		break;
	case 0x400E:
		break;
	case 0x400F:
		break;

		// DMC (IRQ, direct load, address, length, status, frame counter).
	case 0x4010:
		break;
	case 0x4011:
		break;
	case 0x4012:
		break;
	case 0x4013:
		break;

		// Status.
	case 0x4015:
		_pulse1.enabled = value & 1;
		_pulse2.enabled = (value >> 1) & 1;
		break;

		// Frame counter.
	case 0x4017:
		_frameCounter.count = 0;
		_frameCounter.mode = value & 0x80;
		_frameCounter.inhibit = value & 0x40;
		_frameCounter.updated = 1;
		break;
	}
}

void APU::PowerOn()
{
	// TODO
}

void APU::Reset()
{
	// TODO
}

void APU::UpdateEnvelope(Envelope* envelope)
{
	if (envelope->start)
	{
		envelope->start = 0;

		envelope->counter = 15;

		envelope->divider = envelope->volPeriod + 1;
	}
	else
	{
		if (!(--envelope->divider))
		{
			envelope->divider = envelope->volPeriod + 1;

			if (envelope->counter)
			{
				--envelope->counter;
			}
			else if (envelope->loopHalt)
			{
				envelope->counter = 15;
			}
		}
	}

	if (envelope->constVol)
	{
		envelope->out = envelope->volPeriod;
	}
	else
	{
		envelope->out = envelope->counter;
	}
}

void APU::UpdateCounter(PulseChannel* pulse)
{
	if (pulse->enabled)
	{
		if (pulse->counter && (!pulse->envelope.loopHalt))
		{
			--pulse->counter;
		}
	}
	else
	{
		pulse->counter = 0;
	}
}

void APU::UpdateSweep(PulseChannel* pulse, uint16_t offset)
{
	if (pulse->sweepDivider)
	{
		pulse->sweepDivider;
		if (pulse->sweepReload)
		{
			pulse->sweepReload = 0;
			pulse->sweepDivider = pulse->sweepPeriod + 1;
		}
	}
	else
	{
		if (pulse->sweepEnable && pulse->sweepShift)
		{
			pulse->sweepDivider = pulse->sweepPeriod + 1;

			pulse->timerPeriod = pulse->sweepTarget;
			pulse->CalcSweep(offset);
		}
	}
}

void APU::QuarterFrame()
{
	// Envelope.
	UpdateEnvelope(&_pulse1.envelope);

	UpdateEnvelope(&_pulse2.envelope);
}

void APU::HalfFrame()
{
	// Process length counters of pulses, triangle, noise, and DMC.
	UpdateCounter(&_pulse1);

	UpdateCounter(&_pulse2);

	// Update sweep.
	UpdateSweep(&_pulse1, _pulse1Offset);

	UpdateSweep(&_pulse2);
}

void APU::Tick()
{
	// TODO

	// Clock frame counter (http://wiki.nesdev.com/w/index.php/APU_Frame_Counter).
	// The sequencer is clocked on every other CPU cycle, so 2 CPU cycles = 1 APU cycle.
	if (_cycleCounter % 2 == 0 || _frameCounter.updated)
	{
		if (_frameCounter.mode)
		{
			// Mode 1: 5-Step Sequence.

			if (_frameCounter.updated || _frameCounter.count == 7456 || _frameCounter.count == 18640)
			{
				QuarterFrame();
				HalfFrame();

				if (_frameCounter.updated)
				{
					_frameCounter.updated = 0;
				}
			}
			else if (_frameCounter.count == 3728 || _frameCounter.count == 11185)
			{
				QuarterFrame();
			}

			if (_frameCounter.count == 18640)
			{
				_frameCounter.count = 0;
			}
			else
			{
				_frameCounter.count++;
			}
		}
		else
		{
			// Mode 0: 4 - Step Sequence.

			if (_frameCounter.updated || _frameCounter.count == 7456 || _frameCounter.count == 14914)
			{
				QuarterFrame();
				HalfFrame();

				if (_frameCounter.updated)
				{
					_frameCounter.updated = 0;
				}
			}
			else if (_frameCounter.count == 3728 || _frameCounter.count == 11185)
			{
				QuarterFrame();
			}

			if (_frameCounter.count == 14914)
			{
				_frameCounter.count = 0;
			}
			else
			{
				_frameCounter.count++;
			}
		}
	}

	// Process timers for all waves.
	if (_cycleCounter % 2 == 0)
	{
		if (_pulse1.timer)
		{
			_pulse1.timer--;
		}
		else
		{
			_pulse1.timer = _pulse1.timerPeriod;

			if (_pulse1.phase)
			{
				_pulse1.phase--;
			}
			else
			{
				_pulse1.phase = 7;
			}
		}

		if (_pulse2.timer)
		{
			_pulse2.timer--;
		}
		else
		{
			_pulse2.timer = _pulse2.timerPeriod;

			if (_pulse2.phase)
			{
				_pulse2.phase--;
			}
			else
			{
				_pulse2.phase = 7;
			}
		}
	}

	// Determine audio output.
	uint8_t pulse1Pos = 0;

	if (_pulse1.counter && !_pulse1.sweepSilence)
	{
		pulse1Pos = outputWaveform[_pulse1.duty][_pulse1.phase] * _pulse1.envelope.out * (_pulse1.counter > 0);
	}

	uint8_t pulse2Pos = 0;

	if (_pulse2.counter && !_pulse2.sweepSilence)
	{
		pulse2Pos = outputWaveform[_pulse2.duty][_pulse2.phase] * _pulse2.envelope.out * (_pulse2.counter > 0);
	}

	// Store current value to calculate average later.
	_averageBuf[_cycleCounter] = (uint16_t)(_squareTable[pulse1Pos + pulse2Pos]);

	// APU works in one frequency, while our PC wants to play audio in another,
	// so we need to adjust both frequencies.
	if (++_cycleCounter % _cycleLimit == 0)
	{
		_cycleCounter = 0;

		// Prevent buffer overflow.
		if (++_buffer_index >= buf_size)
		{
			_buffer_index = 0;
		}

		// Get average for output buffer.
		for (int i = 0; i < _cycleLimit; i++)
		{
			_averageSum += _averageBuf[i];
		}

		_outBuf[_buffer_index] = (uint16_t)(_averageSum / _cycleLimit);

		_averageSum = 0;
	}
}

long APU::ReadSamples(uint16_t* buf, long bufSize)
{
	int currentSize = 0;

	if (_buffer_index > 0 && _buffer_index >= bufSize)
	{
		currentSize = bufSize < _buffer_index ? bufSize : _buffer_index;

		memcpy(buf, _outBuf, currentSize * sizeof(uint16_t));

		_buffer_index = 0;
	}

	return currentSize;
}
