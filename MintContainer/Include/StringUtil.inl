#pragma once


namespace mint
{
#pragma region U8CharCodeViewer
    MINT_INLINE bool U8CharCodeViewer::operator!=(const U8CharCodeViewer& rhs) const
    {
        return _string[_byteAt] != rhs._string[rhs._byteAt];
    }

    MINT_INLINE U8CharCode U8CharCodeViewer::operator*() const noexcept
    {
        return StringUtil::encode(_string, _byteAt);
    }

    MINT_INLINE U8CharCodeViewer U8CharCodeViewer::operator++()
    {
        if (_string != nullptr && _string[_byteAt] != 0)
        {
            _byteAt += StringUtil::countByteInCharCode(_string, _byteAt);
        }
        return (*this);
    }

    MINT_INLINE U8CharCodeViewer U8CharCodeViewer::begin() const
    {
        return U8CharCodeViewer(_string);
    }

    MINT_INLINE U8CharCodeViewer U8CharCodeViewer::end() const
    {
        constexpr char8_t endString[1]{};
        return U8CharCodeViewer(endString);
    }
#pragma endregion


    namespace StringUtil
    {
        template<typename T>
        MINT_INLINE constexpr bool isNullOrEmpty(const T* const string)
        {
            return (string == nullptr || string[0] == 0);
        }

		constexpr bool is7BitASCII(const char8_t* const string)
		{
			for (uint32 at = 0; string[at] != 0; ++at)
			{
                if ((string[at] >> 7) & 1)
                {
                    return false;
                }
			}
            return true;
		}

        MINT_INLINE constexpr uint32 countByte(const char8_t* const string)
        {
            if (string == nullptr)
            {
                return 0;
            }

            for (uint32 at = 0; ; ++at)
            {
                if (string[at] == 0)
                {
                    return at;
                }
            }
        }

        MINT_INLINE constexpr uint32 countByteInCharCode(const U8CharCode u8CharCode)
        {
            //                       RESULT == ((x >> 3) ^ 1) + (x >> 3) * ( 1 + ((x + 1) >> 4) + ((x >> 2) & 1) + ((x >> 1) & 1) )
            //                      ----------------------------------------------------------------------------------------------
            // x == 1111 ==  15   =>    4   == (      0     ) + (   1  ) * ( 1 + (      1     ) + (      1     ) + (      1     ) )
            // x == 1110 ==  14   =>    3   == (      0     ) + (   1  ) * ( 1 + (      0     ) + (      1     ) + (      1     ) )
            // x == 110? == 12~13 =>    2   == (      0     ) + (   1  ) * ( 1 + (      0     ) + (      1     ) + (      0     ) )
            // x == 0??? ==  0~7  =>    1   == (      1     ) + (   0  ) * ( 1 + (      0     ) + (      ?     ) + (      ?     ) )
            const char8_t head = u8CharCode & 0xFF;
            const char8_t x = head >> 4;
            return ((x >> 3) ^ 1) + (x >> 3) * ( 1 + ((x + 1) >> 4) + ((x >> 2) & 1) + ((x >> 1) & 1) );
        }

        MINT_INLINE constexpr uint32 countByteInCharCode(const char8_t* const string, const uint32 byteAt)
        {
            if (string == nullptr)
            {
                return 0;
            }
            return countByteInCharCode(string[byteAt]);
        }

        MINT_INLINE uint32 length(const char* const string)
        {
            if (string == nullptr)
            {
                return 0;
            }
            return static_cast<uint32>(::strlen(string));
        }

        MINT_INLINE uint32 length(const wchar_t* const string)
        {
            if (string == nullptr)
            {
                return 0;
            }
            return static_cast<uint32>(::wcslen(string));
        }
        
        MINT_INLINE constexpr uint32 length(const char8_t* const string)
        {
            if (string == nullptr)
            {
                return 0;
            }

            uint32 at = 0;
            for (uint32 length = 0; ; ++length)
            {
                if (string[at] == 0)
                {
                    return length;
                }

                at += countByteInCharCode(string[at]);
            }
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
            // nullptr == nullptr
            // ptr == ptr
            if (a == b)
            {
                return true;
            }
            // either a or b is nullptr
            if (a == nullptr || b == nullptr)
            {
                return false;
            }
            return (::strcmp(a, b) == 0);
        }
        
        MINT_INLINE bool compare(const wchar_t* const a, const wchar_t* const b)
        {
            // nullptr == nullptr
            // ptr == ptr
            if (a == b)
            {
                return true;
            }
            // either a or b is nullptr
            if (a == nullptr || b == nullptr)
            {
                return false;
            }
            return (::wcscmp(a, b) == 0);
        }
        
        MINT_INLINE constexpr bool compare(const char8_t* const a, const char8_t* const b)
		{
            // nullptr == nullptr
            // ptr == ptr
            if (a == b)
            {
                return true;
            }
            // either a or b is nullptr
            if (a == nullptr || b == nullptr)
            {
                return false;
            }
            uint32 at = 0;
			for (; a[at] != 0; ++at)
			{
                if (a[at] != b[at])
                {
                    return false;
                }
			}
            return (b[at] == 0);
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
            ::wcscpy_s(dest, source);
        }
    }
}
