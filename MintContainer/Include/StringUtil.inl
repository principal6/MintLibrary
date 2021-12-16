#include "StringUtil.h"
#pragma once


namespace mint
{
    namespace StringUtil
    {
        MINT_INLINE const bool isNullOrEmpty(const char* const rawString)
        {
            return (rawString == nullptr || rawString[0] == '\0');
        }

        MINT_INLINE const bool isNullOrEmpty(const wchar_t* const rawWideString)
        {
            return (rawWideString == nullptr || rawWideString[0] == L'\0');
        }

        MINT_INLINE const uint32 length(const char* const rawString)
        {
            if (isNullOrEmpty(rawString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::strlen(rawString));
        }

        MINT_INLINE const uint32 length(const wchar_t* const rawWideString)
        {
            if (isNullOrEmpty(rawWideString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::wcslen(rawWideString));
        }

        template <typename T>
        MINT_INLINE const uint32 find(const T* const source, const T* const target, const uint32 offset)
        {
            if (source == nullptr || target == nullptr)
            {
                return kStringNPos;
            }

            const uint32 sourceLength = StringUtil::length(source);
            const uint32 targetLength = StringUtil::length(target);
            if (sourceLength < offset + targetLength)
            {
                return kStringNPos;
            }

            uint32 result = kStringNPos;
            uint32 targetIter = 0;
            for (uint32 sourceIter = offset; sourceIter < sourceLength; ++sourceIter)
            {
                if (source[sourceIter] == target[targetIter])
                {
                    if (targetIter == 0)
                    {
                        result = sourceIter;
                    }

                    ++targetIter;
                    if (targetIter == targetLength)
                    {
                        break;
                    }
                }
                else
                {
                    targetIter = 0;
                    result = kStringNPos;
                }
            }

            return result;
        }

        MINT_INLINE const bool compare(const char* const a, const char* const b)
        {
            return (::strcmp(a, b) == 0);
        }
        
        MINT_INLINE const bool compare(const wchar_t* const a, const wchar_t* const b)
        {
            return (::wcscmp(a, b) == 0);
        }

        template<uint32 DestSize>
        MINT_INLINE void copy(char(&dest)[DestSize], const char* const source)
        {
            ::strcpy_s(dest, source);
        }

        template<uint32 DestSize>
        MINT_INLINE void copy(wchar_t(&dest)[DestSize], const wchar_t* const source)
        {
            ::strcpy_s(dest, source);
        }
    }
}
