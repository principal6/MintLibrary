#pragma once


#ifndef FS_STRING_UTIL_H
#define FS_STRING_UTIL_H


#include <Container\Vector.h>
#include <Container\DynamicString.h>


namespace fs
{
	namespace StringUtil
	{
        FS_INLINE bool isNullOrEmpty(const char* const rawString)
		{
			return (nullptr == rawString || '\0' == rawString[0]);
		}

        static uint64 hashRawString64(const char* rawString)
        {
            // Hashing algorithm: FNV1a

            if (isNullOrEmpty(rawString) == true)
            {
                return kUint64Max;
            }

            static constexpr uint64 kOffset = 0xcbf29ce484222325;
            static constexpr uint64 kPrime  = 0x00000100000001B3;
            const size_t rawStringLength = strlen(rawString);

            uint64 hash = kOffset;
            for (size_t rawStringAt = 0; rawStringAt < rawStringLength; ++rawStringAt)
            {
                hash ^= static_cast<uint8>(rawString[rawStringAt]);
                hash *= kPrime;
            }
            return hash;
        }

        FS_INLINE uint32 strlen(const char* const rawString)
        {
            if (isNullOrEmpty(rawString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::strlen(rawString));
        }

        static void tokenize(const fs::DynamicStringA& inputString, const char delimiter, fs::Vector<fs::DynamicStringA>& outArray);
	}
}


#endif // !FS_STRING_UTIL_H
