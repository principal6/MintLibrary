#include <FsLogger/Include/Logger.h>

#include <Windows.h>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <cstdarg>


namespace fs
{
	namespace Logger
	{
		static constexpr uint32 kTimeBufferSize = 100;
		static constexpr uint32 kFinalBufferSize = 1024;

		void logInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char(&outBuffer)[kFinalBufferSize]);

		void setOutputFileName(const char* const fileName)
		{
			std::lock_guard<std::mutex> scopeLock{ _mutex };

			_outputFileName = fileName;
		}

		void log(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
		{
			static char finalBuffer[kFinalBufferSize]{};
			static char content[kFinalBufferSize]{};

			// variadic arguments
			{
				va_list vl;
				va_start(vl, format);
				vsprintf_s(content, kFinalBufferSize, format, vl);
				va_end(vl);
			}

			logInternal(logTag, author, content, functionName, fileName, lineNumber, finalBuffer);

			printf(finalBuffer);
		}

		void logError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
		{
			static constexpr int32 kErrorExitCode = -1;
			static char finalBuffer[kFinalBufferSize]{};
			static char content[kFinalBufferSize]{};

			// variadic arguments
			{
				va_list vl;
				va_start(vl, format);
				vsprintf_s(content, kFinalBufferSize, format, vl);
				va_end(vl);
			}

			logInternal(logTag, author, content, functionName, fileName, lineNumber, finalBuffer);

			printf(finalBuffer);
			::MessageBoxA(nullptr, finalBuffer, "LOG ERROR", MB_ICONERROR);

#if defined FS_DEBUG
			DebugBreak();
#else
			exit(kErrorExitCode);
#endif
		}

		void logInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char(&outBuffer)[kFinalBufferSize])
		{
			static char timeBuffer[kTimeBufferSize]{};
			{
				std::lock_guard<std::mutex> scopeLock{ _mutex };

				const time_t now = time(nullptr);
				tm localNow;
				localtime_s(&localNow, &now);
				strftime(timeBuffer, kTimeBufferSize, "%Y-%m-%d-%H:%M:%S", &localNow);

				sprintf_s(outBuffer, kFinalBufferSize, "[%s] %s [%s] %s : %s[%d] %s()\n", logTag, timeBuffer, author, content, fileName + _basePathOffset, lineNumber, functionName);

				OutputDebugStringA(outBuffer);

				_history.append(outBuffer);
			}
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	using namespace fs::Logger;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_basePathOffset = 0;

		if (true == _isFirstTime)
		{
			_isFirstTime = false;

			std::filesystem::path currentPath = std::filesystem::current_path();
			if (currentPath.has_parent_path())
			{
				currentPath = currentPath.parent_path();
			}
			if (currentPath.has_parent_path())
			{
				currentPath = currentPath.parent_path();
			}
			_basePathOffset = static_cast<uint32>(currentPath.string().length());
		}

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		std::lock_guard<std::mutex> scopeLock{ _mutex };
		if (_outputFileName.empty() == false)
		{
			std::ofstream ofs;
			ofs.open(_outputFileName.c_str());

			ofs.write(_history.c_str(), _history.length());

			ofs.close();
		}
		break;
	}
	return TRUE;
}
