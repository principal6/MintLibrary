#pragma once


namespace mint
{
	namespace Profiler
	{
		MINT_INLINE uint64 GetCurrentTimeMs() noexcept
		{
			return ScopedCPUProfiler::ScopedCPUProfilerLogger::GetCurrentTimeMs();
		}

		MINT_INLINE uint64 GetCurrentTimeUs() noexcept
		{
			return ScopedCPUProfiler::ScopedCPUProfilerLogger::GetCurrentTimeUs();
		}


		inline uint64 FPSCounter::_previousTimeUs = 0;
		inline uint64 FPSCounter::_frameTimeUs = 0;
		inline uint64 FPSCounter::_previousFpsTimeUs = 0;
		inline uint64 FPSCounter::_frameCounter = 0;
		inline uint64 FPSCounter::_fps = 0;
		MINT_INLINE void FPSCounter::Count() noexcept
		{
			++_frameCounter;

			const uint64 currentTimeUs = GetCurrentTimeUs();
			if (currentTimeUs - _previousFpsTimeUs >= 1'000'000)
			{
				_fps = _frameCounter;
				_frameCounter = 0;
				_previousFpsTimeUs = currentTimeUs;
			}

			_frameTimeUs = currentTimeUs - _previousTimeUs;
			_previousTimeUs = currentTimeUs;
		}

		MINT_INLINE uint64 FPSCounter::GetFPS() noexcept
		{
			return _fps;
		}

		inline uint64 FPSCounter::GetFrameTimeUs() noexcept
		{
			return _frameTimeUs;
		}

		inline double FPSCounter::GetFrameTimeMs() noexcept
		{
			return (_frameTimeUs / 1'000.0);
		}


		inline ScopedCPUProfiler::LogData::LogData(const std::string& content, const uint64 startTimepointMs, const uint64 durationMs)
			: _content{ content }
			, _startTimepointMs{ startTimepointMs }
			, _durationMs{ durationMs }
		{

		}

		inline ScopedCPUProfiler::ScopedCPUProfiler(const std::string& content)
			: _startTimepointMs{ ScopedCPUProfilerLogger::GetCurrentTimeMs() }
			, _content{ content }
		{
			__noop;
		}

		inline ScopedCPUProfiler::~ScopedCPUProfiler()
		{
			const uint64 durationMs = ScopedCPUProfilerLogger::GetCurrentTimeMs() - _startTimepointMs;
			ScopedCPUProfilerLogger::GetInstance().Log(*this, durationMs);
		}

		MINT_INLINE const std::vector<ScopedCPUProfiler::LogData>& ScopedCPUProfiler::GetEntireLogData() noexcept
		{
			return ScopedCPUProfilerLogger::GetInstance().GetLogData();
		}

		MINT_INLINE ScopedCPUProfiler::ScopedCPUProfilerLogger& ScopedCPUProfiler::ScopedCPUProfilerLogger::GetInstance() noexcept
		{
			static ScopedCPUProfilerLogger instance;
			return instance;
		}

		MINT_INLINE uint64 ScopedCPUProfiler::ScopedCPUProfilerLogger::GetCurrentTimeMs() noexcept
		{
			static std::chrono::steady_clock steadyClock;
			return std::chrono::duration_cast<std::chrono::milliseconds>(steadyClock.now().time_since_epoch()).count();
		}

		MINT_INLINE uint64 ScopedCPUProfiler::ScopedCPUProfilerLogger::GetCurrentTimeUs() noexcept
		{
			static std::chrono::steady_clock steadyClock;
			return std::chrono::duration_cast<std::chrono::microseconds>(steadyClock.now().time_since_epoch()).count();
		}

		MINT_INLINE void ScopedCPUProfiler::ScopedCPUProfilerLogger::Log(const ScopedCPUProfiler& profiler, const uint64 durationMs) noexcept
		{
			_logArray.emplace_back(profiler._content, profiler._startTimepointMs, durationMs);
		}

		MINT_INLINE const std::vector<ScopedCPUProfiler::LogData>& ScopedCPUProfiler::ScopedCPUProfilerLogger::GetLogData() const noexcept
		{
			return _logArray;
		}
	}
}
