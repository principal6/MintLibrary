#include <MintCommon/Include/Logger.h>

#include <Windows.h>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <cstdarg>


namespace mint
{
#pragma region LoggerString
    LoggerString::LoggerString()
        : _capacity{ 0 }
        , _size{ 0 }
        , _rawPointer{ nullptr }
    {
        reserve(2048);
    }

    LoggerString::~LoggerString()
    {
        release();
    }

    LoggerString& LoggerString::operator=(const char* const rhs)
    {
        const uint32 rhsLength = static_cast<uint32>(::strlen(rhs));
        reserve(rhsLength + 1);
        ::strcpy_s(_rawPointer, _capacity, rhs);
        _size = rhsLength;
        return *this;
    }

    LoggerString& LoggerString::operator+=(const char* const rhs)
    {
        const uint32 rhsLength = static_cast<uint32>(::strlen(rhs));
        reserve(max(_capacity * 2, _size + rhsLength + 1));
        ::strcpy_s(&_rawPointer[_size], rhsLength + 1, rhs);
        _size += rhsLength;
        return *this;
    }

    void LoggerString::reserve(const uint32 newCapacity) noexcept
    {
        if (newCapacity <= _capacity)
        {
            return;
        }

        char* temp = nullptr;
        if (0 < _size)
        {
            temp = MINT_NEW_ARRAY(char, _size + 1);
            ::strcpy_s(temp, _size + 1, _rawPointer);
        }
        
        MINT_DELETE_ARRAY(_rawPointer);
        _rawPointer = MINT_NEW_ARRAY(char, newCapacity);
        
        if (0 < _size)
        {
            ::strcpy_s(_rawPointer, _size + 1, temp);
            MINT_DELETE_ARRAY(temp);
        }

        _capacity = newCapacity;
    }

    void LoggerString::release() noexcept
    {
        MINT_DELETE_ARRAY(_rawPointer);
        _capacity = 0;
        _size = 0;
    }
#pragma endregion


#pragma region Logger
    Logger::Logger()
        : _basePathOffset{ 0 }
    {
        std::filesystem::path currentPath = std::filesystem::current_path();
        //if (currentPath.has_parent_path())
        //{
        //    currentPath = currentPath.parent_path();
        //}
        _basePathOffset = static_cast<uint32>(currentPath.string().length()) + 1;
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

    void Logger::setOutputFileName(const char* const fileName) noexcept
    {
        Logger& logger = getInstance();
        std::lock_guard<std::mutex> scopeLock{ logger._mutex };
        logger._outputFileName = fileName;
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

    void Logger::logAlert(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
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
        ::MessageBoxA(nullptr, content, "LOG ALERT", MB_ICONEXCLAMATION);
    }

    void Logger::logError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...)
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
        ::MessageBoxA(nullptr, content, "LOG ERROR", MB_ICONERROR);
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
                if (author == nullptr || ::strlen(author) == 0)
                {
                    sprintf_s(outBuffer, kFinalBufferSize, "%s\n", content);
                }
                else
                {
                    sprintf_s(outBuffer, kFinalBufferSize, "[%s] %s\n", author, content);
                }
            }
            else
            {
                const uint32 fileNameLength = static_cast<uint32>(::strlen(fileName));
                sprintf_s(outBuffer, kFinalBufferSize, "%s(%d): %s() - [%s] %s [%s] %s \n", (_basePathOffset < fileNameLength) ? fileName + _basePathOffset : fileName,
                    lineNumber, functionName, logTag, timeBuffer, author, content);
            }

            OutputDebugStringA(outBuffer);

            _history += outBuffer;
        }
    }
#pragma endregion
}
