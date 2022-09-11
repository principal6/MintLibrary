#pragma once


namespace mint
{
#pragma region U8Viewer
    MINT_INLINE bool U8Viewer::operator!=(const U8Viewer& rhs) const
    {
        return _string[_byteAt] != rhs._string[rhs._byteAt];
    }

    MINT_INLINE U8CharCode U8Viewer::operator*() const noexcept
    {
        return StringUtil::encode(_string, _byteAt);
    }

    MINT_INLINE U8Viewer U8Viewer::operator++()
    {
        if (_string != nullptr && _string[_byteAt] != 0)
        {
            _byteAt += StringUtil::countCharByte(_string, _byteAt);
        }
        return (*this);
    }

    MINT_INLINE U8Viewer U8Viewer::begin() const
    {
        return U8Viewer(_string);
    }

    MINT_INLINE U8Viewer U8Viewer::end() const
    {
        constexpr char8_t endString[1]{};
        return U8Viewer(endString);
    }
#pragma endregion


    namespace StringUtil
    {
        template<typename T>
        MINT_INLINE bool isNullOrEmpty(const T* const rawString)
        {
            return (rawString == nullptr || rawString[0] == 0);
        }

        MINT_INLINE constexpr uint32 countByte(const char8_t* const string)
        {
            for (uint32 at = 0; ; ++at)
            {
                if (string[at] == 0)
                {
                    return at;
                }
            }
        }

        MINT_INLINE constexpr uint32 countCharByte(const U8CharCode u8CharCode)
        {
            const char8_t head = u8CharCode & 0xFF;
            if (head & 0b10000000)
            {
                // 1111 ^ 1111 = 0000
                // 1110 ^ 1111 = 0001
                // 1100 ^ 1111 = 0011
                const char8_t x = (head >> 4) ^ 0b1111;
                return (x == 0 ? 4 : (x == 1 ? 3 : 2));
            }
            return 0;
        }

        MINT_INLINE constexpr uint32 countCharByte(const char8_t* const string, const uint32 byteAt)
        {
            if (string == nullptr)
            {
                return 0;
            }

            const char8_t head = string[byteAt];
            if (head & 0b10000000)
            {
                // 1111 ^ 1111 = 0000
                // 1110 ^ 1111 = 0001
                // 1100 ^ 1111 = 0011
                const char8_t x = (head >> 4) ^ 0b1111;
                return (x == 0 ? 4 : (x == 1 ? 3 : 2));
            }
            return 0;
        }

        MINT_INLINE uint32 length(const char* const rawString)
        {
            if (isNullOrEmpty(rawString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::strlen(rawString));
        }

        MINT_INLINE uint32 length(const wchar_t* const rawWideString)
        {
            if (isNullOrEmpty(rawWideString) == true)
            {
                return 0;
            }
            return static_cast<uint32>(::wcslen(rawWideString));
        }

        template <typename T>
        MINT_INLINE uint32 find(const T* const source, const T* const target, uint32 offset)
        {
            if (source == nullptr || target == nullptr)
            {
                return kStringNPos;
            }

            uint32 sourceLength = StringUtil::length(source);
            uint32 targetLength = StringUtil::length(target);
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

        MINT_INLINE bool compare(const char* const a, const char* const b)
        {
            return (::strcmp(a, b) == 0);
        }
        
        MINT_INLINE bool compare(const wchar_t* const a, const wchar_t* const b)
        {
            return (::wcscmp(a, b) == 0);
        }

        template<uint32 DestSize>
        MINT_INLINE void copy(char8_t(&dest)[DestSize], const char8_t* const source)
        {
            ::strcpy_s(dest, source);
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
