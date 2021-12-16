#include "ScopedCpuProfiler.h"
#pragma once


namespace mint
{
    namespace Profiler
    {
        MINT_INLINE const uint64 getCurrentTimeMs() noexcept
        {
            return ScopedCpuProfiler::ScopedCpuProfilerLogger::getCurrentTimeMs();
        }

        MINT_INLINE const uint64 getCurrentTimeUs() noexcept
        {
            return ScopedCpuProfiler::ScopedCpuProfilerLogger::getCurrentTimeUs();
        }


        inline uint64 FpsCounter::_previousTimeUs       = 0;
        inline uint64 FpsCounter::_frameTimeUs          = 0;
        inline uint64 FpsCounter::_previousFpsTimeUs    = 0;
        inline uint64 FpsCounter::_frameCounter         = 0;
        inline uint64 FpsCounter::_fps                  = 0;
        MINT_INLINE void FpsCounter::count() noexcept
        {
            ++_frameCounter;

            const uint64 currentTimeUs = getCurrentTimeUs();
            if (currentTimeUs - _previousFpsTimeUs >= 1'000'000)
            {
                _fps = _frameCounter;
                _frameCounter = 0;
                _previousFpsTimeUs = currentTimeUs;
            }

            _frameTimeUs = currentTimeUs - _previousTimeUs;
            _previousTimeUs = currentTimeUs;
        }

        MINT_INLINE const uint64 FpsCounter::getFps() noexcept
        {
            return _fps;
        }

        inline const uint64 FpsCounter::getFrameTimeUs() noexcept
        {
            return _frameTimeUs;
        }

        inline const uint64 FpsCounter::getFrameTimeMs() noexcept
        {
            return (_frameTimeUs / 1'000);
        }


        inline ScopedCpuProfiler::Log::Log(const std::string& content, const uint64 startTimepointMs, const uint64 durationMs)
            : _content{ content }
            , _startTimepointMs{ startTimepointMs }
            , _durationMs{ durationMs }
        {

        }

        inline ScopedCpuProfiler::ScopedCpuProfiler(const std::string& content)
            : _startTimepointMs{ ScopedCpuProfilerLogger::getCurrentTimeMs() }
            , _content{ content }
        {
            __noop;
        }

        inline ScopedCpuProfiler::~ScopedCpuProfiler()
        {
            const uint64 durationMs = ScopedCpuProfilerLogger::getCurrentTimeMs() - _startTimepointMs;
            ScopedCpuProfilerLogger::getInstance().log(*this, durationMs);
        }

        MINT_INLINE const std::vector<ScopedCpuProfiler::Log>& ScopedCpuProfiler::getEntireLogArray() noexcept
        {
            return ScopedCpuProfilerLogger::getInstance().getLogArray();
        }

        MINT_INLINE ScopedCpuProfiler::ScopedCpuProfilerLogger& ScopedCpuProfiler::ScopedCpuProfilerLogger::getInstance() noexcept
        {
            static ScopedCpuProfilerLogger instance;
            return instance;
        }

        MINT_INLINE const uint64 ScopedCpuProfiler::ScopedCpuProfilerLogger::getCurrentTimeMs() noexcept
        {
            static std::chrono::steady_clock steadyClock;
            return std::chrono::duration_cast<std::chrono::milliseconds>(steadyClock.now().time_since_epoch()).count();
        }

        MINT_INLINE const uint64 ScopedCpuProfiler::ScopedCpuProfilerLogger::getCurrentTimeUs() noexcept
        {
            static std::chrono::steady_clock steadyClock;
            return std::chrono::duration_cast<std::chrono::microseconds>(steadyClock.now().time_since_epoch()).count();
        }

        MINT_INLINE void ScopedCpuProfiler::ScopedCpuProfilerLogger::log(const ScopedCpuProfiler& profiler, const uint64 durationMs) noexcept
        {
            _logArray.emplace_back(profiler._content, profiler._startTimepointMs, durationMs);
        }

        MINT_INLINE const std::vector<ScopedCpuProfiler::Log>& ScopedCpuProfiler::ScopedCpuProfilerLogger::getLogArray() const noexcept
        {
            return _logArray;
        }
    }
}
