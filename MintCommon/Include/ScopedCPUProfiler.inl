#pragma once


namespace mint
{
	namespace Profiler
	{
		MINT_INLINE uint64 getCurrentTimeMs() noexcept
		{
			return ScopedCPUProfiler::ScopedCPUProfilerLogger::getCurrentTimeMs();
		}

		MINT_INLINE uint64 getCurrentTimeUs() noexcept
		{
			return ScopedCPUProfiler::ScopedCPUProfilerLogger::getCurrentTimeUs();
		}


		inline uint64 FPSCounter::_previousTimeUs = 0;
		inline uint64 FPSCounter::_frameTimeUs = 0;
		inline uint64 FPSCounter::_previousFpsTimeUs = 0;
		inline uint64 FPSCounter::_frameCounter = 0;
		inline uint64 FPSCounter::_fps = 0;
		MINT_INLINE void FPSCounter::count() noexcept
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

		MINT_INLINE uint64 FPSCounter::getFps() noexcept
		{
			return _fps;
		}

		inline uint64 FPSCounter::getFrameTimeUs() noexcept
		{
			return _frameTimeUs;
		}

		inline double FPSCounter::getFrameTimeMs() noexcept
		{
			return (_frameTimeUs / 1'000.0);
		}


		inline ScopedCPUProfiler::Log::Log(const std::string& content, const uint64 startTimepointMs, const uint64 durationMs)
			: _content{ content }
			, _startTimepointMs{ startTimepointMs }
			, _durationMs{ durationMs }
		{

		}

		inline ScopedCPUProfiler::ScopedCPUProfiler(const std::string& content)
			: _startTimepointMs{ ScopedCPUProfilerLogger::getCurrentTimeMs() }
			, _content{ content }
		{
			__noop;
		}

		inline ScopedCPUProfiler::~ScopedCPUProfiler()
		{
			const uint64 durationMs = ScopedCPUProfilerLogger::getCurrentTimeMs() - _startTimepointMs;
			ScopedCPUProfilerLogger::getInstance().log(*this, durationMs);
		}

		MINT_INLINE const std::vector<ScopedCPUProfiler::Log>& ScopedCPUProfiler::getEntireLogArray() noexcept
		{
			return ScopedCPUProfilerLogger::getInstance().getLogArray();
		}

		MINT_INLINE ScopedCPUProfiler::ScopedCPUProfilerLogger& ScopedCPUProfiler::ScopedCPUProfilerLogger::getInstance() noexcept
		{
			static ScopedCPUProfilerLogger instance;
			return instance;
		}

		MINT_INLINE uint64 ScopedCPUProfiler::ScopedCPUProfilerLogger::getCurrentTimeMs() noexcept
		{
			static std::chrono::steady_clock steadyClock;
			return std::chrono::duration_cast<std::chrono::milliseconds>(steadyClock.now().time_since_epoch()).count();
		}

		MINT_INLINE uint64 ScopedCPUProfiler::ScopedCPUProfilerLogger::getCurrentTimeUs() noexcept
		{
			static std::chrono::steady_clock steadyClock;
			return std::chrono::duration_cast<std::chrono::microseconds>(steadyClock.now().time_since_epoch()).count();
		}

		MINT_INLINE void ScopedCPUProfiler::ScopedCPUProfilerLogger::log(const ScopedCPUProfiler& profiler, const uint64 durationMs) noexcept
		{
			_logArray.emplace_back(profiler._content, profiler._startTimepointMs, durationMs);
		}

		MINT_INLINE const std::vector<ScopedCPUProfiler::Log>& ScopedCPUProfiler::ScopedCPUProfilerLogger::getLogArray() const noexcept
		{
			return _logArray;
		}
	}
}
