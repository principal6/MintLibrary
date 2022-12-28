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
		static uint64 getCurrentTimeMs() noexcept;
		static uint64 getCurrentTimeUs() noexcept;


		class FPSCounter
		{
		private:
			FPSCounter() = default;
			~FPSCounter() = default;

		public:
			static void count() noexcept;
			static uint64 getFps() noexcept;
			static uint64 getFrameTimeUs() noexcept;
			static uint64 getFrameTimeMs() noexcept;

		private:
			static uint64 _previousTimeUs;
			static uint64 _frameTimeUs;
			static uint64 _previousFpsTimeUs;
			static uint64 _frameCounter;
			static uint64 _fps;
		};


		class ScopedCPUProfiler
		{
			friend uint64 getCurrentTimeMs() noexcept;
			friend uint64 getCurrentTimeUs() noexcept;

		public:
			struct Log
			{
			public:
				Log() = default;
				Log(const std::string& content, const uint64 startTimepointMs, const uint64 durationMs);
				~Log() = default;

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
				static ScopedCPUProfilerLogger& getInstance() noexcept;
				static uint64 getCurrentTimeMs() noexcept;
				static uint64 getCurrentTimeUs() noexcept;

			public:
				void log(const ScopedCPUProfiler& profiler, const uint64 durationMs) noexcept;
				const std::vector<Log>& getLogArray() const noexcept;

			private:
				std::vector<Log> _logArray;
			};

		public:
			ScopedCPUProfiler(const std::string& content);
			~ScopedCPUProfiler();

		public:
			static const std::vector<Log>& getEntireLogArray() noexcept;

		private:
			uint64  _startTimepointMs;
			std::string _content;
		};
	}
}


#include <MintCommon/Include/ScopedCPUProfiler.inl>


#endif // !_MINT_COMMON_SCOPED_CPU_PROFILER_H_
