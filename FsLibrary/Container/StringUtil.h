#pragma once


#ifndef FS_STRING_UTIL_H
#define FS_STRING_UTIL_H


#include <FsLibrary/ContiguousContainer/ContiguousVector.h>
#include <FsLibrary/ContiguousContainer/ContiguousString.h>


namespace fs
{
	namespace StringUtil
	{
        FS_INLINE const bool isNullOrEmpty(const char* const rawString)
		{
			return (nullptr == rawString || '\0' == rawString[0]);
		}

        FS_INLINE const bool isNullOrEmpty(const wchar_t* const rawWideString)
        {
            return (nullptr == rawWideString || L'\0' == rawWideString[0]);
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

        FS_INLINE uint32 wcslen(const wchar_t* const rawWideString)
        {
            if (isNullOrEmpty(rawWideString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::wcslen(rawWideString));
        }

        FS_INLINE const bool strcmp(const char* const a, const char* const b)
        {
            return (0 == ::strcmp(a, b));
        }

        void convertWideStringToString(const std::wstring& source, std::string& destination);
        void excludeExtension(std::string& inoutText);

        static void tokenize(const fs::ContiguousStringA& inputString, const char delimiter, fs::ContiguousVector<fs::ContiguousStringA>& outArray);
        static void tokenize(const fs::ContiguousStringA& inputString, const fs::ContiguousVector<char>& delimiterArray, fs::ContiguousVector<fs::ContiguousStringA>& outArray);
        static void tokenize(const std::string& inputString, const std::string& delimiterString, std::vector<std::string>& outArray);
    }
}


#endif // !FS_STRING_UTIL_H
