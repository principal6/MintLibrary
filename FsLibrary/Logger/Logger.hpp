#pragma once


#include <Logger/Logger.h>

#include <File\TextFile.h>

#include <ctime>
#include <filesystem>
#include <string>


namespace fs
{
	inline Logger::Logger()
		: _basePathOffset{ 0 }
	{
		__noop;
	}

	inline Logger::~Logger()
	{
		std::lock_guard<std::mutex> scopeLock{ _mutex };
		if (_outputFileName.empty() == false)
		{
			fs::TextFileWriter textFileWriter;
			textFileWriter.write(_history.c_str());
			textFileWriter.save(_outputFileName.c_str());
		}
	}

	inline void Logger::setOutputFileName(const char* const fileName)
	{
		Logger& logger = getInstance();
		{
			std::lock_guard<std::mutex> scopeLock{ logger._mutex };

			logger._outputFileName = fileName;
		}
	}
	inline void Logger::log(const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber)
	{
		static char finalBuffer[kFinalBufferSize]{};

		logInternal(" LOG ", author, content, functionName, fileName, lineNumber, finalBuffer);

		printf(finalBuffer);
	}

	inline void Logger::logError(const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber)
	{
		static constexpr int32 kErrorExitCode = -1;
		static char finalBuffer[kFinalBufferSize]{};

		logInternal(" ERR ", author, content, functionName, fileName, lineNumber, finalBuffer);

		printf(finalBuffer);
		::MessageBoxA(nullptr, finalBuffer, "LOG ERROR", MB_ICONERROR);
		
#if defined FS_DEBUG
		DebugBreak();
#else
		exit(kErrorExitCode);
#endif
	}

	inline void Logger::logInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char (&outBuffer)[kFinalBufferSize])
	{
		static char timeBuffer[kTimeBufferSize]{};
		Logger& logger = getInstance();
		{
			std::lock_guard<std::mutex> scopeLock{ logger._mutex };

			const time_t now = time(nullptr);
			tm localNow;
			localtime_s(&localNow, &now);
			strftime(timeBuffer, kTimeBufferSize, "%Y-%m-%d-%H:%M:%S", &localNow);

			sprintf_s(outBuffer, kFinalBufferSize, "[%s] [%s] %s - %s %s[%d] %s()\n", logTag, author, content, timeBuffer, fileName + logger._basePathOffset, lineNumber, functionName);

			logger._history.append(outBuffer);
		}
	}

	inline Logger& Logger::getInstance() noexcept
	{
		static bool isFirstTime = true;
		static Logger logger = Logger();
		if (true == isFirstTime)
		{
			isFirstTime = false;

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
