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
#define FS_LOG(author, content) fs::Logger::log(author, content, __func__, __FILE__, __LINE__)
#define FS_LOG_ERROR(author, content) fs::Logger::logError(author, content, __func__, __FILE__, __LINE__)


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
		static void		log(const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber);
		static void		logError(const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber);

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
