#pragma once


#ifndef _MINT_CONTAINER_STRING_UTIL_HPP_
#define _MINT_CONTAINER_STRING_UTIL_HPP_


#include <Windows.h>

#include <varargs.h>

#include <MintContainer/Include/StringUtil.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Tree.hpp>
#include <MintContainer/Include/StringReference.hpp>


namespace mint
{
	template <uint32 BufferSize>
	MINT_INLINE void formatString(char(&buffer)[BufferSize], const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vsprintf_s(buffer, format, args);
		va_end(args);
	}

	MINT_INLINE void formatString(char* const buffer, const uint32 bufferSize, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vsprintf_s(buffer, bufferSize, format, args);
		va_end(args);
	}

	MINT_INLINE void formatString(StringA& buffer, const uint32 bufferSize, const char* format, ...)
	{
		buffer.resize(bufferSize);

		va_list args;
		va_start(args, format);
		::vsprintf_s(&buffer[0], bufferSize, format, args);
		va_end(args);
	}

	template <uint32 BufferSize>
	MINT_INLINE void formatString(StackStringA<BufferSize>& buffer, const char* format, ...)
	{
		StackStringA<BufferSize> temp;

		va_list args;
		va_start(args, format);
		::vsprintf_s(temp.data(), BufferSize, format, args);
		va_end(args);

		buffer.resize(temp.length());
		buffer.assign(temp.c_str());
	}

	template <uint32 BufferSize>
	MINT_INLINE void formatString(wchar_t(&buffer)[BufferSize], const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vswprintf_s(buffer, format, args);
		va_end(args);
	}

	MINT_INLINE void formatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vswprintf_s(buffer, bufferSize, format, args);
		va_end(args);
	}

	MINT_INLINE void formatString(StringW& buffer, const uint32 bufferSize, const wchar_t* format, ...)
	{
		buffer.resize(bufferSize);

		va_list args;
		va_start(args, format);
		::vswprintf_s(&buffer[0], bufferSize, format, args);
		va_end(args);
	}

	template <uint32 BufferSize>
	MINT_INLINE void formatString(StackStringW<BufferSize>& buffer, const wchar_t* format, ...)
	{
		StackStringW<BufferSize> temp;

		va_list args;
		va_start(args, format);
		::vswprintf_s(temp.data(), BufferSize, format, args);
		va_end(args);

		buffer.resize(temp.length());
		buffer.assign(temp.c_str());
	}


	inline StringRange::StringRange()
		: _offset{ 0 }
		, _length{ kUint32Max }
	{
		__noop;
	}

	inline StringRange::StringRange(const uint32 offset)
		: _offset{ offset }
		, _length{ kUint32Max }
	{
		__noop;
	}

	inline StringRange::StringRange(const uint32 offset, const uint32 length)
		: _offset{ offset }
		, _length{ length }
	{
		__noop;
	}

	// 나중에 string 구현하고 나면 없앨 것!!!
	inline StringRange::StringRange(const uint64 offset, const uint32 length)
		: _offset{ static_cast<uint32>(offset) }
		, _length{ length }
	{
	}

	// 나중에 string 구현하고 나면 없앨 것!!!
	inline StringRange::StringRange(const uint64 offset, const uint64 length)
		: _offset{ static_cast<uint32>(offset) }
		, _length{ static_cast<uint32>(length) }
	{
		__noop;
	}

	MINT_INLINE bool StringRange::isLengthSet() const noexcept
	{
		return _length;
	}


	namespace StringUtil
	{
		MINT_INLINE constexpr U8CharCode encode(const char8_t ch)
		{
			return static_cast<U8CharCode>(ch);
		};

		MINT_INLINE constexpr U8CharCode encode(const char8_t(&ch)[2])
		{
			return static_cast<U8CharCode>(ch[0] | (ch[1] << 8));
		}

		MINT_INLINE constexpr U8CharCode encode(const char8_t(&ch)[3])
		{
			return static_cast<U8CharCode>(ch[0] | (ch[1] << 8) | (ch[2] << 16));
		}

		MINT_INLINE constexpr U8CharCode encode(const char8_t(&ch)[4])
		{
			return static_cast<U8CharCode>(ch[0] | (ch[1] << 8) | (ch[2] << 16) | (ch[3] << 24));
		}

		MINT_INLINE constexpr U8CharCode encode(const char8_t* const string, const uint32 byteAt)
		{
			if (string == nullptr)
			{
				return 0;
			}

			const char8_t head = string[byteAt];
			const uint8 leftHalf = ((head & 0b11110000) >> 4);
			U8CharCode charCode = head;
			uint8 byteCount = 1;
			while (byteCount < 4)
			{
				if ((leftHalf & (1 << (3 - byteCount))) == 0)
				{
					break;
				}
				charCode |= (static_cast<U8CharCode>(string[byteAt + byteCount]) << (static_cast<uint64>(byteCount) * 8));
				++byteCount;
			}
			return charCode;
		}

		MINT_INLINE StringU8 decode(const U8CharCode code)
		{
			char8_t ch[4]{ static_cast<uint8>(code), static_cast<uint8>(code >> 8), static_cast<uint8>(code >> 16), static_cast<uint8>(code >> 24) };
			return StringU8(ch);
		}

		MINT_INLINE StringU8 convertWideStringToUTF8(const StringW& source)
		{
			const int length = ::WideCharToMultiByte(CP_UTF8, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0, nullptr, nullptr);
			StringU8 destination(length, 0);
			::WideCharToMultiByte(CP_UTF8, 0, source.c_str(), static_cast<int>(source.length()), reinterpret_cast<char*>(&destination[0]), length, nullptr, nullptr);
			return destination;
		}

		MINT_INLINE StringW convertUTF8ToWideString(const StringU8& source)
		{
			const int length = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(source.c_str()), static_cast<int>(source.length()), nullptr, 0);
			StringW destination(length, 0);
			::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(source.c_str()), static_cast<int>(source.length()), &destination[0], length);
			return destination;
		}

		MINT_INLINE void convertWideStringToString(const StringW& source, StringA& destination)
		{
			const int length = ::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0, nullptr, nullptr);
			destination.resize(length);
			::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()), nullptr, nullptr);
		}

		MINT_INLINE void convertWideStringToString(const std::wstring& source, std::string& destination)
		{
			const int length = ::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0, nullptr, nullptr);
			destination.resize(length);
			::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()), nullptr, nullptr);
		}

		MINT_INLINE void convertStringToWideString(const std::string& source, std::wstring& destination)
		{
			const int length = ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0);
			destination.resize(length);
			::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()));
		}

		MINT_INLINE void convertStringAToStringW(const StringA& source, StringW& destination) noexcept
		{
			const int length = ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0);
			destination.resize(length);
			::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(source.length()));
		}

		MINT_INLINE void convertStringWToStringA(const StringW& source, StringA& destination) noexcept
		{
			const int length = ::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0, nullptr, nullptr);
			destination.resize(length);
			::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()), nullptr, nullptr);
		}

		template<uint32 BufferSize>
		inline void convertStackStringAToStackStringW(const StackStringA<BufferSize>& source, StackStringW<BufferSize>& destination) noexcept
		{
			const int length = ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0);
			destination.resize(length);
			::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(source.length()));
		}

		template<typename T>
		MINT_INLINE uint32 computeExtenstionAt(const StringReference<T>& inoutText)
		{
			const uint32 length = inoutText.length();
			uint32 found = inoutText.find('.', 1);
			while (found < length - 1)
			{
				if (::isalpha(inoutText[found - 1]) == true && ::isalpha(inoutText[found + 1]) == true)
				{
					return found;
				}
				found = inoutText.find('.', found + 1);
			}
			return kStringNPos;
		}

		MINT_INLINE size_t computeExtenstionAt(std::string& inoutText)
		{
			const size_t length = inoutText.size();
			size_t found = inoutText.find('.', 1);
			while (found < length - 1)
			{
				if (::isalpha(inoutText[found - 1]) && ::isalpha(inoutText[found + 1]))
				{
					return found;
				}
				found = inoutText.find('.', found + 1);
			}
			return std::string::npos;
		}

		MINT_INLINE bool hasExtension(std::string& inoutText)
		{
			const size_t found = computeExtenstionAt(inoutText);
			return (found != std::string::npos);
		}

		MINT_INLINE void excludeExtension(std::string& inoutText)
		{
			const size_t found = computeExtenstionAt(inoutText);
			if (found != std::string::npos)
			{
				inoutText = inoutText.substr(0, found);
			}
		}

		template <typename T>
		inline void tokenize(const std::basic_string<T>& inputString, const T delimiter, Vector<std::basic_string<T>>& outTokens)
		{
			Vector<char> delimiters;
			delimiters.push_back(delimiter);
			tokenize(inputString, delimiters, outTokens);
		}

		template <typename T>
		inline void tokenize(const std::basic_string<T>& inputString, const Vector<T>& delimiters, Vector<std::basic_string<T>>& outTokens)
		{
			outTokens.clear();

			if (inputString.empty() == true)
			{
				return;
			}

			uint32 prevAt = 0;
			const uint32 delimiterCount = static_cast<uint32>(delimiters.size());
			const uint32 length = static_cast<uint32>(inputString.length());
			for (uint32 at = 0; at < length; ++at)
			{
				for (uint32 delimiterIndex = 0; delimiterIndex < delimiterCount; ++delimiterIndex)
				{
					if (inputString.at(at) == delimiters.at(delimiterIndex))
					{
						if (prevAt < at)
						{
							outTokens.push_back(inputString.substr(prevAt, at - prevAt));
						}

						prevAt = at + 1;
					}
				}
			}

			if (prevAt < length)
			{
				outTokens.push_back(inputString.substr(prevAt, length - prevAt));
			}
		}

		template<typename INT, typename T>
		inline std::enable_if_t<std::is_integral_v<INT>, void> toString(const INT i, MutableString<T>& outString)
		{
			constexpr uint32 kBufferSize = 32;
			thread_local static char buffer[kBufferSize]{};
			::sprintf_s(buffer, kBufferSize, (std::is_unsigned_v<INT> ? "%llu" : "%lld"), (std::is_unsigned_v<INT> ? static_cast<uint64>(i) : static_cast<int64>(i)));
			outString = reinterpret_cast<T*>(buffer);
		}

		template<typename INT>
		inline std::enable_if_t<std::is_integral_v<INT>, void> toString(const INT i, MutableString<wchar_t>& outString)
		{
			constexpr uint32 kBufferSize = 32;
			thread_local static wchar_t buffer[kBufferSize]{};
			::swprintf_s(buffer, kBufferSize, (std::is_unsigned_v<INT> ? L"%llu" : L"%lld"), (std::is_unsigned_v<INT> ? static_cast<uint64>(i) : static_cast<int64>(i)));
			outString = buffer;
		}

		template<typename FLT, typename T>
		inline std::enable_if_t<std::is_floating_point_v<FLT>, void> toString(const FLT f, MutableString<T>& outString)
		{
			constexpr uint32 kBufferSize = 64;
			thread_local static char buffer[kBufferSize]{};
			::sprintf_s(buffer, kBufferSize, "%f", f);
			outString = reinterpret_cast<T*>(buffer);
		}

		template<typename FLT>
		inline std::enable_if_t<std::is_floating_point_v<FLT>, void> toString(const FLT f, MutableString<wchar_t>& outString)
		{
			constexpr uint32 kBufferSize = 64;
			thread_local static wchar_t buffer[kBufferSize]{};
			::swprintf_s(buffer, kBufferSize, L"%f", f);
			outString = buffer;
		}

		template<typename T>
		inline int32 stringToInt32(const StringReference<T>& string)
		{
			return ::atoi(string.c_str());
		}

		template<typename T>
		inline int64 stringToInt64(const StringReference<T>& string)
		{
			return ::atoll(string.c_str());
		}

		template<typename T>
		inline float stringToFloat(const StringReference<T>& string)
		{
			return static_cast<float>(::atof(string.c_str()));
		}

		template<typename T>
		inline double stringToDouble(const StringReference<T>& string)
		{
			return ::atof(string.c_str());
		}
	}
}


#endif // !_MINT_CONTAINER_STRING_UTIL_HPP_
