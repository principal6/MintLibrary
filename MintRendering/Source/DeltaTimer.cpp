#include <MintRendering/Include/DeltaTimer.h>

#include <MintCommon/Include/ScopedCPUProfiler.h>


namespace mint
{
	namespace Rendering
	{
		DeltaTimer::DeltaTimer()
			: _deltaTimeS{ 0.0f }
			, _prevTimePointUs{ 0 }
		{
			__noop;
		}

		DeltaTimer::~DeltaTimer()
		{
			__noop;
		}

		const DeltaTimer& DeltaTimer::getInstance() noexcept
		{
			static DeltaTimer deltaTimer;
			return deltaTimer;
		}

		float DeltaTimer::computeDeltaTimeSec() const noexcept
		{
			const uint64 currTimePointUs = Profiler::GetCurrentTimeUs();
			const uint64 deltaTimeUs = currTimePointUs - _prevTimePointUs;

			std::scoped_lock<std::mutex> scopedLock{ _mutex };

			_deltaTimeS = deltaTimeUs * 0.000001f;

			_prevTimePointUs = currTimePointUs;

			return _deltaTimeS;
		}

		float DeltaTimer::getDeltaTimeSec() const noexcept
		{
			return _deltaTimeS;
		}

	}
}
