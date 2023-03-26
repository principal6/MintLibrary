#pragma once


#ifndef _MINT_CONTAINER_STRING_UTIL_H_
#define _MINT_CONTAINER_STRING_UTIL_H_


#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/StackString.h>


namespace mint
{
	struct StringRange final
	{
		StringRange();
		StringRange(const uint32 offset);
		StringRange(const uint32 offset, const uint32 length);
		StringRange(const uint64 offset, const uint32 length);
		StringRange(const uint64 offset, const uint64 length);

	public:
		bool IsLengthSet() const noexcept;

	public:
		uint32 _offset;
		uint32 _length;
	};

	class U8CharCodeViewer
	{
	public:
		class ConstIterator;

	public:
		U8CharCodeViewer(const char8_t* const string) : _string{ string } { __noop; }
		~U8CharCodeViewer() = default;

	public:
		ConstIterator begin() const;
		ConstIterator end() const;

	public:
		class ConstIterator
		{
		public:
			ConstIterator(const char8_t* const string, const uint32 byteAt);

		public:
			ConstIterator& operator++();
			U8CharCode operator*() const noexcept;
			bool operator!=(const ConstIterator& rhs) const;

		private:
			const char8_t* const _string;
			uint32 _byteAt;
		};

	private:
		const char8_t* _string;
	};


	template <uint32 BufferSize>
	void FormatString(char(&buffer)[BufferSize], const char* format, ...);
	void FormatString(char* const buffer, const uint32 bufferSize, const char* format, ...);
	void FormatString(StringA& buffer, const uint32 bufferSize, const char* format, ...);
	template <uint32 BufferSize>
	void FormatString(StackStringA<BufferSize>& buffer, const char* format, ...);

	template <uint32 BufferSize>
	void FormatString(wchar_t(&buffer)[BufferSize], const wchar_t* format, ...);
	void FormatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...);
	void FormatString(StringW& buffer, const uint32 bufferSize, const wchar_t* format, ...);
	template <uint32 BufferSize>
	void FormatString(StackStringW<BufferSize>& buffer, const wchar_t* format, ...);


	namespace StringUtil
	{
		template<typename T>
		constexpr bool IsNullOrEmpty(const T* const string);

		constexpr bool Is7BitASCII(const char8_t* const string);

		// returns the count of items in the string
		template <typename T>
		constexpr uint32 Length(const T* const string);

		template <typename T>
		constexpr uint32 CountBytesFromLeadingByte(const T leadingByte);
		constexpr uint32 CountBytesInCharCode(const U8CharCode u8CharCode);

		template <typename T>
		constexpr uint32 GetBytePosition(const T* const string, const uint32 charPosition);

		// The implementation of this function is too naive.
		// Prefer to use utf8 encoding (char8_t).
		constexpr uint32 CountChars(const char* const string);
		constexpr uint32 CountChars(const wchar_t* const string);
		// returns the count of characters in the string
		constexpr uint32 CountChars(const char8_t* const string);

		template<typename T>
		constexpr uint32 Find(const T* const string, const T* const substring, const uint32 offset = 0);
		
		template<typename T>
		constexpr uint32 FindLastOf(const T* const string, const T* const substring, const uint32 offset = 0);
		
		template<typename T>
		constexpr bool Contains(const T* const string, const T* const substring);

		template <typename T>
		constexpr bool Equals(const T* const a, const T* const b);

		template<uint32 DestSize>
		void Copy(char8_t(&dest)[DestSize], const char8_t* const source);
		template<uint32 DestSize>
		void Copy(char(&dest)[DestSize], const char* const source);
		template<uint32 DestSize>
		void Copy(wchar_t(&dest)[DestSize], const wchar_t* const source);
		template<typename T>
		void Copy(T* dest, const T* const source, const uint32 byteCount);

		constexpr U8CharCode Encode(const char8_t ch);
		constexpr U8CharCode Encode(const char8_t(&ch)[2]);
		constexpr U8CharCode Encode(const char8_t(&ch)[3]);
		constexpr U8CharCode Encode(const char8_t(&ch)[4]);
		constexpr U8CharCode Encode(const char8_t* const string, const uint32 byteAt);
		StringU8 Decode(const U8CharCode code);

		StringU8 ConvertWideStringToUTF8(const StringW& source);
		StringW ConvertUTF8ToWideString(const StringU8& source);
		void ConvertWideStringToString(const StringW& source, StringA& destination);
		void ConvertWideStringToString(const std::wstring& source, std::string& destination);
		void ConvertStringToWideString(const std::string& source, std::wstring& destination);

		void ConvertStringAToStringW(const StringA& source, StringW& destination) noexcept;
		void ConvertStringWToStringA(const StringW& source, StringA& destination) noexcept;

		template <uint32 BufferSize>
		void ConvertStackStringAToStackStringW(const StackStringA<BufferSize>& source, StackStringW<BufferSize>& destination) noexcept;

		void ExcludeExtension(std::string& inoutText);

		template <typename T>
		void Tokenize(const std::basic_string<T>& inputString, const T delimiter, Vector<std::basic_string<T>>& outTokens);
		template <typename T>
		void Tokenize(const std::basic_string<T>& inputString, const Vector<T>& delimiters, Vector<std::basic_string<T>>& outTokens);

		template<typename INT, typename T>
		std::enable_if_t<std::is_integral_v<INT>, void> ToString(const INT i, MutableString<T>& outString);
		template<typename FLT, typename T>
		std::enable_if_t<std::is_floating_point_v<FLT>, void> ToString(const FLT f, MutableString<T>& outString);

		template <typename ValueType, typename CharType, typename = void>
		struct isToStringAvailable : std::false_type {};

		template <typename ValueType, typename CharType>
		struct isToStringAvailable<ValueType, CharType,
			std::void_t<decltype(ToString(std::declval<const ValueType>(), std::declval<mint::MutableString<CharType>&>()))>> : std::true_type {};

		template<typename T>
		int32 StringToInt32(const StringReference<T>& string);
		template<typename T>
		int64 StringToInt64(const StringReference<T>& string);
		template<typename T>
		float StringToFloat(const StringReference<T>& string);
		template<typename T>
		double StringToDouble(const StringReference<T>& string);
	}
}


#include <MintContainer/Include/StringUtil.inl>


#endif // !_MINT_CONTAINER_STRING_UTIL_H_
