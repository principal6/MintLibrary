#pragma once


#ifndef MINT_LOGGER_H
#define MINT_LOGGER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <mutex>
#include <string>


//#define MINT_LOG_FOR_ASSURE_SILENT


namespace mint
{
    static constexpr int32 kErrorExitCode = -1;

#pragma region Logging
    #if defined MINT_DEBUG
        #define _MINT_LOG_ERROR_ACTION __debugbreak()
    #else
        #define _MINT_LOG_ERROR_ACTION exit(kErrorExitCode)
    #endif

    #define MINT_LOG_UNTAGGED(author, format, ...)              mint::Logger::getInstance().log(nullptr, author, nullptr, nullptr, 0, format, __VA_ARGS__)
    #define MINT_LOG(author, format, ...)                       mint::Logger::getInstance().log(" _LOG_ ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
    #define MINT_LOG_ALERT(author, format, ...)                 mint::Logger::getInstance().logAlert(" ALERT ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
    #define MINT_LOG_ERROR(author, format, ...)                 mint::Logger::getInstance().logError(" ERROR ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__); _MINT_LOG_ERROR_ACTION
#pragma endregion


#pragma region Assertion
    #define MINT_NEVER                                      { mint::Logger::getInstance().logError(" ASSUR ", "XXX", __func__, __FILE__, __LINE__, "THIS BRANCH IS NOT ALLOWED!"); _MINT_LOG_ERROR_ACTION; }
    // [DESCRIPTION]
    // Return false if expression is false!
    #define MINT_ASSURE(expression)                         if (!(expression)) { mint::Logger::getInstance().logError(" ASSUR ", "XXX", __func__, __FILE__, __LINE__, "NOT ASSURED. RETURN FALSE!"); _MINT_LOG_ERROR_ACTION; return false; }

#if defined MINT_LOG_FOR_ASSURE_SILENT
    #define MINT_ASSURE_SILENT(expression)                  if (!(expression)) { mint::Logger::getInstance().log(" ASSUR ", "XXX", __func__, __FILE__, __LINE__, "NOT ASSURED. RETURN FALSE!"); return false; }
#else
    #define MINT_ASSURE_SILENT(expression)                  if (!(expression)) { return false; }
#endif

    #if defined MINT_DEBUG
        #define MINT_ASSERT(author, expression, format, ...)    if (!(expression)) { mint::Logger::getInstance().logError(" ASSRT ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__); _MINT_LOG_ERROR_ACTION; }
    #else
        #define MINT_ASSERT(author, expression, format, ...)
    #endif
    #define MINT_RETURN_FALSE_IF_NOT(expression)                if (!(expression)) return false
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
        void            logAlert(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);
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


#endif // !MINT_LOGGER_H
