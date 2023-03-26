#pragma once


namespace mint
{
#pragma region U8CharCodeViewer
	MINT_INLINE U8CharCodeViewer::ConstIterator U8CharCodeViewer::begin() const
	{
		return ConstIterator(_string, 0);
	}

	MINT_INLINE U8CharCodeViewer::ConstIterator U8CharCodeViewer::end() const
	{
		static constexpr const char8_t kNullString[2]{};
		return ConstIterator(kNullString, 0);
	}

	inline U8CharCodeViewer::ConstIterator::ConstIterator(const char8_t* const string, const uint32 byteAt)
		: _string{ string }
		, _byteAt{ byteAt }
	{
		__noop;
	}

	MINT_INLINE U8CharCodeViewer::ConstIterator& U8CharCodeViewer::ConstIterator::operator++()
	{
		if (_string != nullptr && _string[_byteAt] != 0)
		{
			_byteAt += StringUtil::CountBytesInCharCode(_string[_byteAt]);
		}
		return (*this);
	}

	MINT_INLINE U8CharCode U8CharCodeViewer::ConstIterator::operator*() const noexcept
	{
		return StringUtil::Encode(_string, _byteAt);
	}

	MINT_INLINE bool U8CharCodeViewer::ConstIterator::operator!=(const ConstIterator& rhs) const
	{
		return _string[_byteAt] != rhs._string[rhs._byteAt];
	}
#pragma endregion


	namespace StringUtil
	{
		template<typename T>
		MINT_INLINE constexpr bool IsNullOrEmpty(const T* const string)
		{
			return (string == nullptr || string[0] == 0);
		}

		MINT_INLINE constexpr bool Is7BitASCII(const char8_t* const string)
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

		template<typename T>
		MINT_INLINE constexpr uint32 Length(const T* const string)
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

		template <typename T>
		MINT_INLINE constexpr uint32 CountBytesFromLeadingByte(const T leadingByte)
		{
			return 1;
		}

		template <>
		MINT_INLINE constexpr uint32 CountBytesFromLeadingByte(const wchar_t leadingByte)
		{
			return 2;
		}

		template <>
		MINT_INLINE constexpr uint32 CountBytesFromLeadingByte(const char8_t leadingByte)
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
		MINT_INLINE constexpr uint32 CountBytesFromLeadingByte(const char leadingByte)
		{
			return 1 + ((leadingByte >> 7) & 1);
		}

		MINT_INLINE constexpr uint32 CountBytesInCharCode(const U8CharCode u8CharCode)
		{
			return CountBytesFromLeadingByte(static_cast<char8_t>(u8CharCode & 0xFF));
		}

		template <typename T>
		MINT_INLINE constexpr uint32 GetBytePosition(const T* const string, const uint32 charPosition)
		{
			if (charPosition == 0)
			{
				return 0;
			}

			uint32 characterPositionCmp = 0;
			for (uint32 bytePosition = 0; string[bytePosition] != 0;)
			{
				bytePosition += CountBytesFromLeadingByte<T>(string[bytePosition]);
				++characterPositionCmp;

				if (characterPositionCmp == charPosition)
				{
					return bytePosition;
				}
			}
			return 0;
		}

		template <>
		MINT_INLINE constexpr uint32 GetBytePosition(const wchar_t* const string, const uint32 characterPosition)
		{
			return characterPosition * 2;
		}

		MINT_INLINE constexpr uint32 CountChars(const char* const string)
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

		MINT_INLINE constexpr uint32 CountChars(const wchar_t* const string)
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

		MINT_INLINE constexpr uint32 CountChars(const char8_t* const string)
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

				at += CountBytesInCharCode(string[at]);
			}
		}

		template <typename T>
		MINT_INLINE constexpr uint32 Find(const T* const string, const T* const substring, uint32 offset)
		{
			const uint32 stringLength = StringUtil::Length(string);
			const uint32 substringLength = StringUtil::Length(substring);
			if (stringLength == 0 || substringLength == 0)
			{
				// Invalid string/substring
				return kStringNPos;
			}
			if (stringLength < offset + substringLength)
			{
				// Insufficient string length
				return kStringNPos;
			}

			uint32 stringAtCache = 0;
			uint32 substringAt = 0;
			for (uint32 stringAt = offset; stringAt < stringLength; __noop)
			{
				if (string[stringAt] == substring[substringAt])
				{
					if (substringAt == 0)
					{
						stringAtCache = stringAt;
					}

					++substringAt;
					++stringAt;

					if (stringAt == stringLength && substringAt < substringLength)
					{
						// Insufficient string length
						return kStringNPos;
					}

					if ((string[stringAt] | substring[substringAt]) == 0)
					{
						return stringAtCache;
					}
				}
				else
				{
					if (substringAt == substringLength)
					{
						return stringAtCache;
					}

					substringAt = 0;
					++stringAt;
				}
			}
			return kStringNPos;
		}

		template <typename T>
		MINT_INLINE constexpr bool Contains(const T* const string, const T* const substring)
		{
			return StringUtil::Find(string, substring, 0) != kStringNPos;
		}

		template <typename T>
		MINT_INLINE constexpr bool Equals(const T* const a, const T* const b)
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
		MINT_INLINE void Copy(char8_t(&dest)[DestSize], const char8_t* const source)
		{
			if (source == nullptr)
			{
				return;
			}
			const uint32 length = Min(DestSize - 1, StringUtil::Length(source));
			::memcpy_s(dest, sizeof(char8_t) * DestSize, source, sizeof(char8_t) * length);
			dest[length] = 0;
		}

		template<uint32 DestSize>
		MINT_INLINE void Copy(char(&dest)[DestSize], const char* const source)
		{
			if (source == nullptr)
			{
				return;
			}
			::strcpy_s(dest, source);
		}

		template<uint32 DestSize>
		MINT_INLINE void Copy(wchar_t(&dest)[DestSize], const wchar_t* const source)
		{
			if (source == nullptr)
			{
				return;
			}
			::wcscpy_s(dest, source);
		}

		template<typename T>
		MINT_INLINE void Copy(T* dest, const T* const source, const uint32 byteCount)
		{
			if (dest == nullptr || source == nullptr)
			{
				return;
			}
			::memcpy(dest, source, sizeof(T) * byteCount);
		}
	}
}
