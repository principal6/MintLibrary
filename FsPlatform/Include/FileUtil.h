#pragma once


namespace fs
{
	namespace FileUtil
	{
		const bool isFile(const char* const fileName) noexcept;
		const bool isDirectory(const char* const fileName) noexcept;
		const bool isReadOnly(const char* const fileName) noexcept;
		const bool exists(const char* const fileName) noexcept;
		void unsetReadOnly(const char* const fileName) noexcept;
		const bool deleteFile(const char* const fileName) noexcept;
	}
}
