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
	MINT_INLINE void FormatString(char(&buffer)[BufferSize], const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vsprintf_s(buffer, format, args);
		va_end(args);
	}

	MINT_INLINE void FormatString(char* const buffer, const uint32 bufferSize, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vsprintf_s(buffer, bufferSize, format, args);
		va_end(args);
	}

	MINT_INLINE void FormatString(StringA& buffer, const uint32 bufferSize, const char* format, ...)
	{
		buffer.Resize(bufferSize);

		va_list args;
		va_start(args, format);
		::vsprintf_s(&buffer[0], bufferSize, format, args);
		va_end(args);
	}

	template <uint32 BufferSize>
	MINT_INLINE void FormatString(StackStringA<BufferSize>& buffer, const char* format, ...)
	{
		StackStringA<BufferSize> temp;

		va_list args;
		va_start(args, format);
		::vsprintf_s(temp.Data(), BufferSize, format, args);
		va_end(args);

		buffer.Resize(temp.Length());
		buffer.Assign(temp.CString());
	}

	template <uint32 BufferSize>
	MINT_INLINE void FormatString(wchar_t(&buffer)[BufferSize], const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vswprintf_s(buffer, format, args);
		va_end(args);
	}

	MINT_INLINE void FormatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);
		::vswprintf_s(buffer, bufferSize, format, args);
		va_end(args);
	}

	MINT_INLINE void FormatString(StringW& buffer, const uint32 bufferSize, const wchar_t* format, ...)
	{
		buffer.Resize(bufferSize);

		va_list args;
		va_start(args, format);
		::vswprintf_s(&buffer[0], bufferSize, format, args);
		va_end(args);
	}

	template <uint32 BufferSize>
	MINT_INLINE void FormatString(StackStringW<BufferSize>& buffer, const wchar_t* format, ...)
	{
		StackStringW<BufferSize> temp;

		va_list args;
		va_start(args, format);
		::vswprintf_s(temp.Data(), BufferSize, format, args);
		va_end(args);

		buffer.Resize(temp.Length());
		buffer.Assign(temp.CString());
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

	inline StringRange::StringRange(const uint32 offset, const uint32 Length)
		: _offset{ offset }
		, _length{ Length }
	{
		__noop;
	}

	// 나중에 string 구현하고 나면 없앨 것!!!
	inline StringRange::StringRange(const uint64 offset, const uint32 Length)
		: _offset{ static_cast<uint32>(offset) }
		, _length{ Length }
	{
	}

	// 나중에 string 구현하고 나면 없앨 것!!!
	inline StringRange::StringRange(const uint64 offset, const uint64 Length)
		: _offset{ static_cast<uint32>(offset) }
		, _length{ static_cast<uint32>(Length) }
	{
		__noop;
	}

	MINT_INLINE bool StringRange::IsLengthSet() const noexcept
	{
		return _length;
	}


	namespace StringUtil
	{
		MINT_INLINE constexpr U8CharCode Encode(const char8_t ch)
		{
			return static_cast<U8CharCode>(ch);
		};

		MINT_INLINE constexpr U8CharCode Encode(const char8_t(&ch)[2])
		{
			return static_cast<U8CharCode>(ch[0] | (ch[1] << 8));
		}

		MINT_INLINE constexpr U8CharCode Encode(const char8_t(&ch)[3])
		{
			return static_cast<U8CharCode>(ch[0] | (ch[1] << 8) | (ch[2] << 16));
		}

		MINT_INLINE constexpr U8CharCode Encode(const char8_t(&ch)[4])
		{
			return static_cast<U8CharCode>(ch[0] | (ch[1] << 8) | (ch[2] << 16) | (ch[3] << 24));
		}

		MINT_INLINE constexpr U8CharCode Encode(const char8_t* const string, const uint32 byteAt)
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

		MINT_INLINE StringU8 Decode(const U8CharCode code)
		{
			char8_t ch[4]{ static_cast<uint8>(code), static_cast<uint8>(code >> 8), static_cast<uint8>(code >> 16), static_cast<uint8>(code >> 24) };
			return StringU8(ch);
		}

		MINT_INLINE StringU8 ConvertWideStringToUTF8(const StringW& source)
		{
			const int Length = ::WideCharToMultiByte(CP_UTF8, 0, source.CString(), static_cast<int>(source.Length()), nullptr, 0, nullptr, nullptr);
			StringU8 destination(Length, 0);
			::WideCharToMultiByte(CP_UTF8, 0, source.CString(), static_cast<int>(source.Length()), reinterpret_cast<char*>(&destination[0]), Length, nullptr, nullptr);
			return destination;
		}

		MINT_INLINE StringW ConvertUTF8ToWideString(const StringU8& source)
		{
			const int Length = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(source.CString()), static_cast<int>(source.Length()), nullptr, 0);
			StringW destination(Length, 0);
			::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(source.CString()), static_cast<int>(source.Length()), &destination[0], Length);
			return destination;
		}

		MINT_INLINE void ConvertWideStringToString(const StringW& source, StringA& destination)
		{
			const int Length = ::WideCharToMultiByte(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), nullptr, 0, nullptr, nullptr);
			destination.Resize(Length);
			::WideCharToMultiByte(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), &destination[0], static_cast<int>(destination.Length()), nullptr, nullptr);
		}

		MINT_INLINE void ConvertWideStringToString(const std::wstring& source, std::string& destination)
		{
			const int Length = ::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0, nullptr, nullptr);
			destination.resize(Length);
			::WideCharToMultiByte(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()), nullptr, nullptr);
		}

		MINT_INLINE void ConvertStringToWideString(const std::string& source, std::wstring& destination)
		{
			const int Length = ::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), nullptr, 0);
			destination.resize(Length);
			::MultiByteToWideChar(CP_ACP, 0, source.c_str(), static_cast<int>(source.length()), &destination[0], static_cast<int>(destination.length()));
		}

		MINT_INLINE void ConvertStringAToStringW(const StringA& source, StringW& destination) noexcept
		{
			const int Length = ::MultiByteToWideChar(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), nullptr, 0);
			destination.Resize(Length);
			::MultiByteToWideChar(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), &destination[0], static_cast<int>(source.Length()));
		}

		MINT_INLINE void ConvertStringWToStringA(const StringW& source, StringA& destination) noexcept
		{
			const int Length = ::WideCharToMultiByte(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), nullptr, 0, nullptr, nullptr);
			destination.Resize(Length);
			::WideCharToMultiByte(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), &destination[0], static_cast<int>(destination.Length()), nullptr, nullptr);
		}

		template<uint32 BufferSize>
		inline void ConvertStackStringAToStackStringW(const StackStringA<BufferSize>& source, StackStringW<BufferSize>& destination) noexcept
		{
			const int Length = ::MultiByteToWideChar(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), nullptr, 0);
			destination.Resize(Length);
			::MultiByteToWideChar(CP_ACP, 0, source.CString(), static_cast<int>(source.Length()), &destination[0], static_cast<int>(source.Length()));
		}

		template<typename T>
		MINT_INLINE uint32 computeExtenstionAt(const StringReference<T>& inoutText)
		{
			const uint32 Length = inoutText.Length();
			uint32 found = inoutText.Find('.', 1);
			while (found < Length - 1)
			{
				if (::isalpha(inoutText[found - 1]) && ::isalpha(inoutText[found + 1]))
				{
					return found;
				}
				found = inoutText.Find('.', found + 1);
			}
			return kStringNPos;
		}

		MINT_INLINE size_t computeExtenstionAt(std::string& inoutText)
		{
			const size_t Length = inoutText.size();
			size_t found = inoutText.find('.', 1);
			while (found < Length - 1)
			{
				if (::isalpha(inoutText[found - 1]) && ::isalpha(inoutText[found + 1]))
				{
					return found;
				}
				found = inoutText.find('.', found + 1);
			}
			return std::string::npos;
		}

		MINT_INLINE bool HasExtension(std::string& inoutText)
		{
			const size_t found = computeExtenstionAt(inoutText);
			return (found != std::string::npos);
		}

		template <typename T>
		MINT_INLINE bool HasExtension(StringReference<T>& inoutText)
		{
			const uint32 found = computeExtenstionAt(inoutText);
			return (found != kStringNPos);
		}

		MINT_INLINE void ExcludeExtension(std::string& inoutText)
		{
			const size_t found = computeExtenstionAt(inoutText);
			if (found != std::string::npos)
			{
				inoutText = inoutText.substr(0, found);
			}
		}

		template <typename T>
		MINT_INLINE void ExcludeExtension(String<T>& inoutText)
		{
			const uint32 found = computeExtenstionAt<T>(inoutText);
			if (found == kStringNPos)
			{
				return;
			}

			inoutText = inoutText.Substring(0, found);
		}

		template <typename T>
		inline void Tokenize(const std::basic_string<T>& inputString, const T delimiter, Vector<std::basic_string<T>>& outTokens)
		{
			Vector<char> delimiters;
			delimiters.PushBack(delimiter);
			Tokenize(inputString, delimiters, outTokens);
		}

		template <typename T>
		inline void Tokenize(const std::basic_string<T>& inputString, const Vector<T>& delimiters, Vector<std::basic_string<T>>& outTokens)
		{
			outTokens.Clear();

			if (inputString.empty() == true)
			{
				return;
			}

			uint32 prevAt = 0;
			const uint32 delimiterCount = static_cast<uint32>(delimiters.Size());
			const uint32 Length = static_cast<uint32>(inputString.length());
			for (uint32 at = 0; at < Length; ++at)
			{
				for (uint32 delimiterIndex = 0; delimiterIndex < delimiterCount; ++delimiterIndex)
				{
					if (inputString.at(at) == delimiters.At(delimiterIndex))
					{
						if (prevAt < at)
						{
							outTokens.PushBack(inputString.substr(prevAt, at - prevAt));
						}

						prevAt = at + 1;
					}
				}
			}

			if (prevAt < Length)
			{
				outTokens.PushBack(inputString.substr(prevAt, Length - prevAt));
			}
		}

		template<typename INT, typename T>
		inline std::enable_if_t<std::is_integral_v<INT>, void> ToString(const INT i, MutableString<T>& outString)
		{
			constexpr uint32 kBufferSize = 32;
			thread_local static char buffer[kBufferSize]{};
			::sprintf_s(buffer, kBufferSize, (std::is_unsigned_v<INT> ? "%llu" : "%lld"), (std::is_unsigned_v<INT> ? static_cast<uint64>(i) : static_cast<int64>(i)));
			outString = reinterpret_cast<T*>(buffer);
		}

		template<typename INT>
		inline std::enable_if_t<std::is_integral_v<INT>, void> ToString(const INT i, MutableString<wchar_t>& outString)
		{
			constexpr uint32 kBufferSize = 32;
			thread_local static wchar_t buffer[kBufferSize]{};
			::swprintf_s(buffer, kBufferSize, (std::is_unsigned_v<INT> ? L"%llu" : L"%lld"), (std::is_unsigned_v<INT> ? static_cast<uint64>(i) : static_cast<int64>(i)));
			outString = buffer;
		}

		template<typename FLT, typename T>
		inline std::enable_if_t<std::is_floating_point_v<FLT>, void> ToString(const FLT f, MutableString<T>& outString)
		{
			constexpr uint32 kBufferSize = 64;
			thread_local static char buffer[kBufferSize]{};
			::sprintf_s(buffer, kBufferSize, "%f", f);
			outString = reinterpret_cast<T*>(buffer);
		}

		template<typename FLT>
		inline std::enable_if_t<std::is_floating_point_v<FLT>, void> ToString(const FLT f, MutableString<wchar_t>& outString)
		{
			constexpr uint32 kBufferSize = 64;
			thread_local static wchar_t buffer[kBufferSize]{};
			::swprintf_s(buffer, kBufferSize, L"%f", f);
			outString = buffer;
		}

		template<typename T>
		inline String<T> ToString(const int64 i)
		{
			constexpr uint32 kBufferSize = 32;
			thread_local static char buffer[kBufferSize]{};
			::sprintf_s(buffer, kBufferSize, "%lld", i);
			return String<T>(reinterpret_cast<T*>(buffer));
		}

		template<>
		inline String<wchar_t> ToString(const int64 i)
		{
			constexpr uint32 kBufferSize = 32;
			thread_local static wchar_t buffer[kBufferSize]{};
			::swprintf_s(buffer, kBufferSize, L"%lld", i);
			return String<wchar_t>(buffer);
		}

		inline StringA ToStringA(const int64 i)
		{
			return ToString<char>(i);
		}

		inline StringW ToStringW(const int64 i)
		{
			return ToString<wchar_t>(i);
		}

		inline StringU8 ToStringU8(const int64 i)
		{
			return ToString<char8_t>(i);
		}

		template<typename T>
		inline String<T> ToString(const double f)
		{
			constexpr uint32 kBufferSize = 64;
			thread_local static char buffer[kBufferSize]{};
			::sprintf_s(buffer, kBufferSize, "%f", f);
			return String<T>(reinterpret_cast<T*>(buffer));
		}

		template<>
		inline String<wchar_t> ToString(const double f)
		{
			constexpr uint32 kBufferSize = 64;
			thread_local static wchar_t buffer[kBufferSize]{};
			::swprintf_s(buffer, kBufferSize, L"%f", f);
			return String<wchar_t>(buffer);
		}

		template<typename T>
		inline int32 StringToInt32(const StringReference<T>& string)
		{
			return ::atoi(reinterpret_cast<const char*>(string.CString()));
		}

		template<>
		inline int32 StringToInt32(const StringReference<wchar_t>& string)
		{
			return ::_wtoi(string.CString());
		}

		template<typename T>
		inline int64 StringToInt64(const StringReference<T>& string)
		{
			return ::atoll(reinterpret_cast<const char*>(string.CString()));
		}

		template<>
		inline int64 StringToInt64(const StringReference<wchar_t>& string)
		{
			return ::_wtoll(string.CString());
		}

		template<typename T>
		inline double StringToDouble(const StringReference<T>& string)
		{
			return ::atof(reinterpret_cast<const char*>(string.CString()));
		}

		template<>
		inline double StringToDouble(const StringReference<wchar_t>& string)
		{
			return ::_wtof(string.CString());
		}

		template<typename T>
		inline float StringToFloat(const StringReference<T>& string)
		{
			return static_cast<float>(StringToDouble(string));
		}

		template<>
		inline float StringToFloat(const StringReference<wchar_t>& string)
		{
			return static_cast<float>(StringToDouble(string));
		}
	}
}


#endif // !_MINT_CONTAINER_STRING_UTIL_HPP_
