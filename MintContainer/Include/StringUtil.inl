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
            _byteAt += StringUtil::computeByteCountInCharCode(_string[_byteAt]);
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

		MINT_INLINE constexpr bool is7BitASCII(const char8_t* const string)
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

		template <typename T>
		MINT_INLINE constexpr uint32 computeCharacterByteSizeFromLeadingByte(const T leadingByte)
		{
			return 1;
		}
		
		template <>
		MINT_INLINE constexpr uint32 computeCharacterByteSizeFromLeadingByte(const wchar_t leadingByte)
		{
			return 2;
		}

		template <>
		MINT_INLINE constexpr uint32 computeCharacterByteSizeFromLeadingByte(const char8_t leadingByte)
		{
			//                       RESULT == ((x >> 3) ^ 1) + (x >> 3) * ( 1 + ((x + 1) >> 4) + ((x >> 2) & 1) + ((x >> 1) & 1) )
			//                      ----------------------------------------------------------------------------------------------
			// x == 1111 ==  15   =>    4   == (      0     ) + (   1  ) * ( 1 + (      1     ) + (      1     ) + (      1     ) )
			// x == 1110 ==  14   =>    3   == (      0     ) + (   1  ) * ( 1 + (      0     ) + (      1     ) + (      1     ) )
			// x == 110? == 12~13 =>    2   == (      0     ) + (   1  ) * ( 1 + (      0     ) + (      1     ) + (      0     ) )
			// x == 0??? ==  0~7  =>    1   == (      1     ) + (   0  ) * ( 1 + (      0     ) + (      ?     ) + (      ?     ) )
			const char8_t x = leadingByte >> 4;
			return ((x >> 3) ^ 1) + (x >> 3) * (1 + ((x + 1) >> 4) + ((x >> 2) & 1) + ((x >> 1) & 1));
		}
        
		template <>
		MINT_INLINE constexpr uint32 computeCharacterByteSizeFromLeadingByte(const char leadingByte)
		{
            return 1 + ((leadingByte >> 7) & 1);
		}

		MINT_INLINE constexpr uint32 computeByteCountInString(const char* const string)
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
		
		MINT_INLINE constexpr uint32 computeByteCountInString(const wchar_t* const string)
		{
			if (string == nullptr)
			{
				return 0;
			}

			for (uint32 at = 0; ; ++at)
			{
				if (string[at] == 0)
				{
					return at * 2;
				}
			}
		}
		
		MINT_INLINE constexpr uint32 computeByteCountInString(const char8_t* const string)
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

		MINT_INLINE constexpr uint32 computeByteCountInCharCode(const U8CharCode u8CharCode)
		{
			return computeCharacterByteSizeFromLeadingByte(static_cast<char8_t>(u8CharCode & 0xFF));
		}

		template <typename T>
		MINT_INLINE constexpr uint32 computeBytePositionFromCharacterPosition(const T* const string, const uint32 characterPosition)
		{
			if (characterPosition == 0)
			{
				return 0;
			}

			uint32 characterPositionCmp = 0;
			for (uint32 bytePosition = 0; string[bytePosition] != 0;)
			{
				bytePosition += computeCharacterByteSizeFromLeadingByte<T>(string[bytePosition]);
				++characterPositionCmp;

				if (characterPositionCmp == characterPosition)
				{
					return bytePosition;
				}
			}
			return 0;
		}

		template <>
		MINT_INLINE constexpr uint32 computeBytePositionFromCharacterPosition(const wchar_t* const string, const uint32 characterPosition)
		{
			return characterPosition * 2;
		}

        MINT_INLINE constexpr uint32 length(const char* const string)
		{
			if (string == nullptr)
			{
				return 0;
			}
			uint32 length = 0;
			for (uint32 at = 0; string[at] != 0; ++at)
			{
                // handle DBCS(Double-Byte Character Sets)
				if ((string[at] >> 7) & 1)
				{
					++at;
				}
				++length;
			}
			return length;
		}

		MINT_INLINE constexpr uint32 length(const wchar_t* const string)
		{
			if (string == nullptr)
			{
				return 0;
			}
			uint32 at = 0;
			for (; string[at] != 0; ++at)
			{
				__noop;
			}
			return at;
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

                at += computeByteCountInCharCode(string[at]);
            }
        }

		template <typename T>
		MINT_INLINE constexpr uint32 find(const T* const string, const T* const substring, uint32 offset)
		{
			if (string == nullptr || substring == nullptr)
			{
				return kStringNPos;
			}

			uint32 stringLength = StringUtil::length(string);
			uint32 substringLength = StringUtil::length(substring);
			if (stringLength < offset + substringLength)
			{
				return kStringNPos;
			}

			const uint32 stringByteOffset = computeBytePositionFromCharacterPosition<T>(string, offset);
			const uint32 stringByteCount = computeByteCountInString(string);
			uint32 result = kStringNPos;
			uint32 stringCharacterByteCount = computeCharacterByteSizeFromLeadingByte<T>(string[stringByteOffset]);
			uint32 substringBytePosition = 0;
			uint32 stringCharacterPosition = offset;
			for (uint32 stringBytePosition = stringByteOffset; stringBytePosition < stringByteCount; __noop)
			{
				if (string[stringBytePosition] == substring[substringBytePosition])
				{
					if (substringBytePosition == 0)
					{
						result = stringCharacterPosition;
					}
					++substringBytePosition;
					++stringBytePosition;
					--stringCharacterByteCount;

					if (stringCharacterByteCount != 0)
					{
						continue;
					}

					if (substringBytePosition > substringLength)
					{
						result = stringCharacterPosition;
						break;
					}

					if ((string[stringBytePosition] | substring[substringBytePosition]) == 0)
					{
						return result;
					}

					stringCharacterByteCount = computeCharacterByteSizeFromLeadingByte<T>(string[stringBytePosition]);
					++stringCharacterPosition;
				}
				else
				{
					if (substringBytePosition == substringLength)
					{
						result = stringCharacterPosition;
						break;
					}

					result = kStringNPos;
					substringBytePosition = 0;
					stringBytePosition += stringCharacterByteCount;

					stringCharacterByteCount = computeCharacterByteSizeFromLeadingByte<T>(string[stringBytePosition]);
					++stringCharacterPosition;
				}
			}
			return result;
		}

		template <>
		MINT_INLINE constexpr uint32 find(const wchar_t* const string, const wchar_t* const substring, uint32 offset)
		{
			if (string == nullptr || substring == nullptr)
			{
				return kStringNPos;
			}

			uint32 stringLength = StringUtil::length(string);
			uint32 substringLength = StringUtil::length(substring);
			if (stringLength < offset + substringLength)
			{
				return kStringNPos;
			}

			uint32 result = kStringNPos;
			uint32 substringCharacterPosition = 0;
			for (uint32 stringCharacterPosition = offset; stringCharacterPosition < stringLength; ++stringCharacterPosition)
			{
				if (string[stringCharacterPosition] == substring[substringCharacterPosition])
				{
					if (substringCharacterPosition == 0)
					{
						result = stringCharacterPosition;
					}

					++substringCharacterPosition;
					if (substringCharacterPosition == substringLength)
					{
						break;
					}
				}
				else
				{
					substringCharacterPosition = 0;
					result = kStringNPos;
				}
			}
			return result;
		}

        template <typename T>
        MINT_INLINE constexpr bool compare(const T* const a, const T* const b)
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
