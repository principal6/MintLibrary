#pragma once


#ifndef FS_LOGGER_H
#define FS_LOGGER_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <mutex>
#include <string>


using int32 = int32_t;
using uint32 = uint32_t;


namespace fs
{
#pragma region Logging
    #define FS_LOG(author, format, ...)                     fs::Logger::getInstance().log(" _LOG_ ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
    #define FS_LOG_PURE(author, format, ...)                fs::Logger::getInstance().log(nullptr, author, nullptr, nullptr, 0, format, __VA_ARGS__)
    #define FS_LOG_ERROR(author, format, ...)               fs::Logger::getInstance().logError(" ERROR ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
#pragma endregion


#pragma region Assertion
    #if defined FS_DEBUG
        #define FS_ASSERT(author, expression, format, ...)  if (!(expression)) fs::Logger::getInstance().logError(" ASSRT ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__);
    #else
        #define FS_ASSERT(author, expression, format, ...)
    #endif
    #define FS_RETURN_FALSE_IF_NOT(expression) if (!(expression)) return false
#pragma endregion


    class Logger
    {
    private:
        static constexpr uint32 kTimeBufferSize = 100;
        static constexpr uint32 kFinalBufferSize = 1024;

    private:
                        Logger();

    public:
                        ~Logger();

    public:
        static Logger&  getInstance() noexcept;
        void            log(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);
        void            logError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);

    private:
        void            logInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char(&outBuffer)[kFinalBufferSize]);
        void            setOutputFileName(const char* const fileName);

    private:
        uint32          _basePathOffset;
        std::mutex      _mutex;
        std::string     _history;
        std::string     _outputFileName;
    };
}


#endif // !FS_LOGGER_H
