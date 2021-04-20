#pragma once


namespace fs
{
    template <uint32 Size>
    FS_INLINE void formatString(char(&buffer)[Size], const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vsprintf_s(buffer, format, args);
        va_end(args);
    }

    FS_INLINE void formatString(char* const buffer, const uint32 bufferSize, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vsprintf_s(buffer, bufferSize, format, args);
        va_end(args);
    }

    template <uint32 Size>
    FS_INLINE void formatString(wchar_t(&buffer)[Size], const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vswprintf_s(buffer, format, args);
        va_end(args);
    }

    FS_INLINE void formatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        ::vswprintf_s(buffer, bufferSize, format, args);
        va_end(args);
    }

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

        FS_INLINE const uint32 strlen(const char* const rawString)
        {
            if (isNullOrEmpty(rawString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::strlen(rawString));
        }

        FS_INLINE const uint32 wcslen(const wchar_t* const rawWideString)
        {
            if (isNullOrEmpty(rawWideString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::wcslen(rawWideString));
        }

        FS_INLINE const uint32 find(const char* const source, const char* const target, const uint32 offset)
        {
            if (source == nullptr || target == nullptr)
            {
                return kStringNPos;
            }

            const uint32 sourceLength = fs::StringUtil::strlen(source);
            const uint32 targetLength = fs::StringUtil::strlen(target);
            if (sourceLength < offset + targetLength)
            {
                return kStringNPos;
            }

            uint32 result = kStringNPos;
            uint32 targetIter = 0;
            bool isFound = false;
            for (uint32 sourceIter = 0; sourceIter < sourceLength; sourceIter++)
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
                        isFound = true;
                        break;
                    }
                }
                else
                {
                    targetIter = 0;
                    result = kStringNPos;
                }
            }

            return (isFound == true) ? result : kStringNPos;
        }

        FS_INLINE const bool strcmp(const char* const a, const char* const b)
        {
            return (0 == ::strcmp(a, b));
        }
    }
}
