#pragma once


#ifndef FS_STRING_UTIL_H
#define FS_STRING_UTIL_H


namespace fs
{
	namespace StringUtil
	{
		inline bool isNullOrEmpty(const char* const rawString)
		{
			return (nullptr == rawString || '\0' == rawString[0]);
		}
	}
}


#endif // !FS_STRING_UTIL_H
