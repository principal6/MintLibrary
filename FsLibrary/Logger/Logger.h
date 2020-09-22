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


	class Logger final
	{
	public:
		Logger(const char* const fileName, const uint32 lineNumber);
		~Logger();

	private:
		Logger();

	public:
		static void		log(const char* const author, const char* const content, const char* const functionName, const char* const fileName, const uint32 lineNumber);

	private:
		static Logger&	getInstance() noexcept;

	private:
		std::mutex*		_mutex;
		uint32			_basePathOffset;
	};
}


#endif // !FS_LOGGER_H
