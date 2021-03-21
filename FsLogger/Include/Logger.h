#pragma once


#ifndef FS_LOGGER_H
#define FS_LOGGER_H


#include <CommonDefinitions.h>

#include <mutex>
#include <string>


using int32 = int32_t;
using uint32 = uint32_t;


#ifdef FS_LOGGER_EXPORT
#define FS_LOGGER_API __declspec(dllexport)
#else
#define FS_LOGGER_API __declspec(dllimport)
#endif


namespace fs
{
#pragma region Logging
	#define FS_LOG(author, format, ...)						fs::Logger::log(" _LOG_ ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
	#define FS_LOG_PURE(author, format, ...)				fs::Logger::log(nullptr, author, nullptr, nullptr, 0, format, __VA_ARGS__)
	#define FS_LOG_ERROR(author, format, ...)				fs::Logger::logError(" ERROR ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
#pragma endregion

#pragma region Assertion
	#if defined FS_DEBUG
		#define FS_ASSERT(author, expression, format, ...)	if (!(expression)) fs::Logger::logError(" ASSRT ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__);
	#else
		#define FS_ASSERT(author, expression, format, ...)
	#endif
	#define FS_RETURN_FALSE_IF_NOT(expression) if (!(expression)) return false
#pragma endregion

	namespace Logger
	{
		FS_LOGGER_API void			setOutputFileName(const char* const fileName);

		FS_LOGGER_API void			log(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);
		FS_LOGGER_API void			logError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);

		FS_LOGGER_API bool			_isFirstTime;
		FS_LOGGER_API uint32		_basePathOffset;
		FS_LOGGER_API std::mutex 	_mutex;
		FS_LOGGER_API std::string	_history;
		FS_LOGGER_API std::string	_outputFileName;
	}
}


#endif // !FS_LOGGER_H
