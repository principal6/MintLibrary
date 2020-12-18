#pragma once


namespace fs
{
	namespace Profiler
	{
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

		FS_INLINE const std::vector<ScopedCpuProfiler::Log>& ScopedCpuProfiler::getEntireLogArray() noexcept
		{
			return ScopedCpuProfilerLogger::getInstance().getLogArray();
		}

		FS_INLINE ScopedCpuProfiler::ScopedCpuProfilerLogger& ScopedCpuProfiler::ScopedCpuProfilerLogger::getInstance() noexcept
		{
			static ScopedCpuProfilerLogger instance;
			return instance;
		}

		FS_INLINE const uint64 ScopedCpuProfiler::ScopedCpuProfilerLogger::getCurrentTimeMs() noexcept
		{
			static std::chrono::steady_clock steadyClock;
			return std::chrono::duration_cast<std::chrono::milliseconds>(steadyClock.now().time_since_epoch()).count();
		}

		FS_INLINE void ScopedCpuProfiler::ScopedCpuProfilerLogger::log(const ScopedCpuProfiler& profiler, const uint64 durationMs) noexcept
		{
			_logArray.emplace_back(profiler._content, profiler._startTimepointMs, durationMs);
		}

		FS_INLINE const std::vector<ScopedCpuProfiler::Log>& ScopedCpuProfiler::ScopedCpuProfilerLogger::getLogArray() const noexcept
		{
			return _logArray;
		}
	}
}
