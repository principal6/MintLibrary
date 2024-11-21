#include <MintApp/Include/DeltaTimer.h>

#include <MintCommon/Include/ScopedCPUProfiler.h>


namespace mint
{
	DeltaTimer::DeltaTimer()
		: _lastFrameNumber{ 0 }
		, _deltaTimeS{ 0.0f }
		, _prevTimePointUs{ Profiler::GetCurrentTimeUs() }
	{
		__noop;
	}

	DeltaTimer::~DeltaTimer()
	{
		__noop;
	}

	const DeltaTimer& DeltaTimer::GetInstance() noexcept
	{
		static DeltaTimer deltaTimer;
		return deltaTimer;
	}

	float DeltaTimer::ComputeDeltaTime(uint64 frameNumber) const noexcept
	{
		const uint64 currTimePointUs = Profiler::GetCurrentTimeUs();
		
		if (frameNumber != _lastFrameNumber)
		{
			const uint64 deltaTimeUs = currTimePointUs - _prevTimePointUs;
			_deltaTimeS = deltaTimeUs * 0.000001f;
			_prevTimePointUs = currTimePointUs;
			_lastFrameNumber = frameNumber;
		}
		return GetDeltaTimeSec();
	}

	float DeltaTimer::GetDeltaTimeSec() const noexcept
	{
		return _deltaTimeS;
	}
}
