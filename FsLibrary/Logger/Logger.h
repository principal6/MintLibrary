#pragma once


#ifndef FS_LOGGER_H
#define FS_LOGGER_H


#include <CommonDefinitions.h>


namespace std
{
	class mutex;
}

namespace fs
{
#pragma region Logging
	#define FS_LOG(author, format, ...)						fs::Logger::log(" _LOG_ ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
	#define FS_LOG_ERROR(author, format, ...)				fs::Logger::logError(" ERROR ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__)
#pragma endregion

#pragma region Assertion
	#if defined FS_DEBUG
		#define FS_ASSERT(author, expression, format, ...)	if (!(expression)) { fs::Logger::logError(" ASSRT ", author, __func__, __FILE__, __LINE__, format, __VA_ARGS__); }
	#else
		#define FS_ASSERT(author, expression, format, ...)
	#endif
#pragma endregion


	class Logger final
	{
		static constexpr uint32 kTimeBufferSize = 100;
		static constexpr uint32 kFinalBufferSize = 500;

	public:
		~Logger();

	private:
		Logger();

	public:
		static void		setOutputFileName(const char* const fileName);

	public:
		static void		log(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);
		static void		logError(const char* const logTag, const char* const author, const char* const functionName, const char* const fileName, const uint32 lineNumber, const char* const format, ...);

	private:
		static void		logInternal(const char* const logTag, const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber, char (&outBuffer)[kFinalBufferSize]);

	private:
		static Logger&	getInstance() noexcept;

	private:
		uint32			_basePathOffset;
		std::mutex		_mutex;
		std::string		_history;
		std::string		_outputFileName;
	};
}


#endif // !FS_LOGGER_H
