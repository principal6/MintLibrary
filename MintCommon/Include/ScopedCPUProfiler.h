#pragma once


#ifndef _MINT_COMMON_SCOPED_CPU_PROFILER_H_
#define _MINT_COMMON_SCOPED_CPU_PROFILER_H_


#include <vector>
#include <string>

#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	namespace Profiler
	{
		static uint64 GetCurrentTimeMs() noexcept;
		static uint64 GetCurrentTimeUs() noexcept;


		class FPSCounter
		{
		private:
			FPSCounter() = default;
			~FPSCounter() = default;

		public:
			static void Count() noexcept;
			static uint64 GetFPS() noexcept;
			static uint64 GetFrameTimeUs() noexcept;
			static double GetFrameTimeMs() noexcept;

		private:
			static uint64 _previousTimeUs;
			static uint64 _frameTimeUs;
			static uint64 _previousFpsTimeUs;
			static uint64 _frameCounter;
			static uint64 _fps;
		};


		class ScopedCPUProfiler
		{
			friend uint64 GetCurrentTimeMs() noexcept;
			friend uint64 GetCurrentTimeUs() noexcept;

		public:
			struct LogData
			{
			public:
				LogData() = default;
				LogData(const std::string& content, const uint64 startTimepointMs, const uint64 durationMs);
				~LogData() = default;

			public:
				std::string _content;
				uint64 _startTimepointMs;
				uint64 _durationMs;
			};

		private:
			class ScopedCPUProfilerLogger
			{
			private:
				ScopedCPUProfilerLogger() = default;
				~ScopedCPUProfilerLogger() = default;

			public:
				static ScopedCPUProfilerLogger& GetInstance() noexcept;
				static uint64 GetCurrentTimeMs() noexcept;
				static uint64 GetCurrentTimeUs() noexcept;

			public:
				void Log(const ScopedCPUProfiler& profiler, const uint64 durationMs) noexcept;
				const std::vector<LogData>& GetLogData() const noexcept;

			private:
				std::vector<LogData> _logArray;
			};

		public:
			ScopedCPUProfiler(const std::string& content);
			~ScopedCPUProfiler();

		public:
			static const std::vector<LogData>& GetEntireLogData() noexcept;

		private:
			uint64  _startTimepointMs;
			std::string _content;
		};
	}
}


#include <MintCommon/Include/ScopedCPUProfiler.inl>


#endif // !_MINT_COMMON_SCOPED_CPU_PROFILER_H_
