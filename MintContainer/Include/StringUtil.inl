#pragma once


namespace mint
{
    namespace StringUtil
    {
        MINT_INLINE const bool isNullOrEmpty(const char* const rawString)
        {
            return (nullptr == rawString || '\0' == rawString[0]);
        }

        MINT_INLINE const bool isNullOrEmpty(const wchar_t* const rawWideString)
        {
            return (nullptr == rawWideString || L'\0' == rawWideString[0]);
        }

        MINT_INLINE const uint32 strlen(const char* const rawString)
        {
            if (isNullOrEmpty(rawString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::strlen(rawString));
        }

        MINT_INLINE const uint32 wcslen(const wchar_t* const rawWideString)
        {
            if (isNullOrEmpty(rawWideString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::wcslen(rawWideString));
        }

        MINT_INLINE const uint32 find(const char* const source, const char* const target, const uint32 offset)
        {
            if (source == nullptr || target == nullptr)
            {
                return kStringNPos;
            }

            const uint32 sourceLength = mint::StringUtil::strlen(source);
            const uint32 targetLength = mint::StringUtil::strlen(target);
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

        MINT_INLINE const bool strcmp(const char* const a, const char* const b)
        {
            return (0 == ::strcmp(a, b));
        }
    }
}
