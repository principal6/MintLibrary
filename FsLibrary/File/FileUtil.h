#pragma once


namespace fs
{
	namespace FileUtil
	{
		inline bool exists(const char* const fileName) noexcept;
		inline bool isReadOnly(const char* const fileName) noexcept;
		inline void unsetReadOnly(const char* const fileName) noexcept;
	}
}
