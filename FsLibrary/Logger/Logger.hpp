#pragma once


#include <Logger/Logger.h>
#include <mutex>
#include <ctime>
#include <filesystem>


namespace fs
{
	inline Logger::Logger()
		: _mutex{ nullptr }
		, _basePathOffset(0)
	{
		__noop;
	}

	inline Logger::Logger(const char* const fileName, const uint32 lineNumber)
		: Logger()
	{
		__noop;
	}

	inline Logger::~Logger()
	{
		FS_DELETE(_mutex);
	}

	inline void Logger::log(const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber)
	{
		static constexpr uint32 kTimeBufferSize = 100;
		static constexpr uint32 kFinalBufferSize = 500;
		static char timeBuffer[kTimeBufferSize]{};
		static char finalBuffer[kFinalBufferSize]{};
		Logger& logger = getInstance();
		{
			std::lock_guard<std::mutex> scopeLock{ *logger._mutex };
			
			const time_t now = time(nullptr);
			tm localNow;
			localtime_s(&localNow, &now);
			strftime(timeBuffer, kTimeBufferSize, "%Y-%m-%d-%H:%M:%S", &localNow);

			sprintf_s(finalBuffer, kFinalBufferSize, "%s %s[%d]::%s() [%s] %s\n", timeBuffer, fileName + logger._basePathOffset, lineNumber, functionName, author, content);
			printf(finalBuffer);
		}
	}

	inline Logger& Logger::getInstance() noexcept
	{
		static bool isFirstTime = true;
		static Logger logger = Logger(__FILE__, __LINE__);
		if (true == isFirstTime)
		{
			isFirstTime = false;
			logger._mutex = FS_NEW(std::mutex);

			std::filesystem::path currentPath = std::filesystem::current_path();
			if (currentPath.has_parent_path())
			{
				currentPath = currentPath.parent_path();
			}
			if (currentPath.has_parent_path())
			{
				currentPath = currentPath.parent_path();
			}
			logger._basePathOffset = static_cast<uint32>(currentPath.string().length());
		}
		return logger;
	}
}
