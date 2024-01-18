#include <MintGame/Include/DeltaTimer.h>

#include <MintCommon/Include/ScopedCPUProfiler.h>


namespace mint
{
	namespace Game
	{
		DeltaTimer::DeltaTimer()
			: _deltaTimeS{ 0.0f }
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

		float DeltaTimer::ComputeDeltaTimeSec() const noexcept
		{
			const uint64 currTimePointUs = Profiler::GetCurrentTimeUs();
			const uint64 deltaTimeUs = currTimePointUs - _prevTimePointUs;

			std::scoped_lock<std::mutex> scopedLock{ _mutex };

			_deltaTimeS = deltaTimeUs * 0.000001f;

			_prevTimePointUs = currTimePointUs;

			return _deltaTimeS;
		}

		float DeltaTimer::GetDeltaTimeSec() const noexcept
		{
			return _deltaTimeS;
		}

	}
}
