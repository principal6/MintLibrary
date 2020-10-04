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

        static uint64 hashRawString64(const char* rawString)
        {
            // Hashing algorithm: FNV1a

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
	}
}


#endif // !FS_STRING_UTIL_H
