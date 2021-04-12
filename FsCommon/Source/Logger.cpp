﻿#include <FsCommon/Include/Logger.h>

#include <Windows.h>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <cstdarg>


namespace fs
{
    Logger::Logger()
        : _basePathOffset{ 0 }
    {
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

    Logger::~Logger()
    {
        std::lock_guard<std::mutex> scopeLock{ _mutex };
        if (_outputFileName.empty() == false)
        {
            std::ofstream ofs;
            ofs.open(_outputFileName.c_str());

            ofs.write(_history.c_str(), _history.length());

            ofs.close();
        }
    }

    Logger& Logger::getInstance() noexcept
    {
        static Logger logger;
        return logger;
    }

    void Logger::setOutputFileName(const char* const fileName)
    {
        std::lock_guard<std::mutex> scopeLock{ _mutex };

        _outputFileName = fileName;
    }

    void Logger::log(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
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

    void Logger::logError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
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

    void Logger::logInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char(&outBuffer)[kFinalBufferSize])
    {
        static char timeBuffer[kTimeBufferSize]{};
        {
            std::lock_guard<std::mutex> scopeLock{ _mutex };

            const time_t now = time(nullptr);
            tm localNow;
            localtime_s(&localNow, &now);
            strftime(timeBuffer, kTimeBufferSize, "%Y-%m-%d-%H:%M:%S", &localNow);

            if (nullptr == logTag || nullptr == functionName || nullptr == fileName)
            {
                sprintf_s(outBuffer, kFinalBufferSize, "[%s] %s\n", author, content);
            }
            else
            {
                sprintf_s(outBuffer, kFinalBufferSize, "[%s] %s [%s] %s : %s[%d] %s()\n", logTag, timeBuffer, author, content, fileName + _basePathOffset, lineNumber, functionName);
            }

            OutputDebugStringA(outBuffer);

            _history.append(outBuffer);
        }
    }
}
