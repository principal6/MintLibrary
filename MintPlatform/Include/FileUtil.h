#pragma once


#ifndef __MINT_PLATFORM_FILE_UTIL_H__
#define __MINT_PLATFORM_FILE_UTIL_H__


namespace mint
{
	namespace FileUtil
	{
		bool IsFile(const char* const fileName) noexcept;
		bool IsDirectory(const char* const fileName) noexcept;
		bool IsReadOnly(const char* const fileName) noexcept;
		void UnsetReadOnly(const char* const fileName) noexcept;
		bool Exists(const char* const path) noexcept;
		bool CreateDirectory_(const char* const directoryName) noexcept;
		bool DeleteFile_(const char* const fileName) noexcept;
	}
}


#endif // !__MINT_PLATFORM_FILE_UTIL_H__
