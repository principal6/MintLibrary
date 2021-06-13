#include <stdafx.h>
#include <MintRendering/Include/DeltaTimer.h>

#include <MintLibrary/Include/Profiler.h>


namespace mint
{
    namespace Rendering
    {
        DeltaTimer::DeltaTimer()
            : _deltaTimeS{ 0.0f }
            , _prevTimePointMs{ 0 }
        {
            __noop;
        }
        
        DeltaTimer::~DeltaTimer()
        {
            __noop;
        }

        const DeltaTimer& DeltaTimer::getDeltaTimer() noexcept
        {
            static DeltaTimer deltaTimer;
            return deltaTimer;
        }

        const float DeltaTimer::computeDeltaTimeS() const noexcept
        {
            const uint64 currTimePointMs = mint::Profiler::getCurrentTimeMs();
            const uint64 deltaTimeMs = currTimePointMs - _prevTimePointMs;
            
            std::scoped_lock<std::mutex> scopedLock{ _mutex };

            _deltaTimeS = deltaTimeMs * 0.001f;

            _prevTimePointMs = currTimePointMs;

            return _deltaTimeS;
        }

        const float DeltaTimer::getDeltaTimeS() const noexcept
        {
            return _deltaTimeS;
        }

    }
}
