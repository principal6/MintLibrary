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
		bool            isLengthSet() const noexcept;

	public:
		uint32          _offset;
		uint32          _length;
	};

	class U8CharCodeViewer
	{
	public:
		class ConstIterator;

	public:
		U8CharCodeViewer(const char8_t* const string) : _string{ string } { __noop; }
		~U8CharCodeViewer() = default;

	public:
		ConstIterator       begin() const;
		ConstIterator       end() const;

	public:
		class ConstIterator
		{
		public:
			ConstIterator(const char8_t* const string, const uint32 byteAt);

		public:
			ConstIterator& operator++();
			U8CharCode      operator*() const noexcept;
			bool            operator!=(const ConstIterator& rhs) const;

		private:
			const char8_t* const    _string;
			uint32                  _byteAt;
		};

	private:
		const char8_t* _string;
	};


	template <uint32 BufferSize>
	void                formatString(char(&buffer)[BufferSize], const char* format, ...);
	void                formatString(char* const buffer, const uint32 bufferSize, const char* format, ...);
	void                formatString(StringA& buffer, const uint32 bufferSize, const char* format, ...);
	template <uint32 BufferSize>
	void                formatString(StackStringA<BufferSize>& buffer, const char* format, ...);

	template <uint32 BufferSize>
	void                formatString(wchar_t(&buffer)[BufferSize], const wchar_t* format, ...);
	void                formatString(wchar_t* const buffer, const uint32 bufferSize, const wchar_t* format, ...);
	void                formatString(StringW& buffer, const uint32 bufferSize, const wchar_t* format, ...);
	template <uint32 BufferSize>
	void                formatString(StackStringW<BufferSize>& buffer, const wchar_t* format, ...);


	namespace StringUtil
	{
		template<typename T>
		constexpr bool      isNullOrEmpty(const T* const string);

		constexpr bool      is7BitASCII(const char8_t* const string);

		// returns the count of items in the string
		template <typename T>
		constexpr uint32    length(const T* const string);

		template <typename T>
		constexpr uint32    countBytesFromLeadingByte(const T leadingByte);
		constexpr uint32    countBytesInCharCode(const U8CharCode u8CharCode);

		template <typename T>
		constexpr uint32    getBytePosition(const T* const string, const uint32 charPosition);

		// The implementation of this function is too naive.
		// Prefer to use utf8 encoding (char8_t).
		constexpr uint32    countChars(const char* const string);
		constexpr uint32    countChars(const wchar_t* const string);
		// returns the count of characters in the string
		constexpr uint32    countChars(const char8_t* const string);

		// here 'offset' refers to character offset (not byte offset!)
		template <typename T>
		constexpr uint32    find(const T* const string, const T* const substring, const uint32 offset = 0);

		template <typename T>
		constexpr bool      compare(const T* const a, const T* const b);

		template<uint32 DestSize>
		void                copy(char8_t(&dest)[DestSize], const char8_t* const source);
		template<uint32 DestSize>
		void                copy(char(&dest)[DestSize], const char* const source);
		template<uint32 DestSize>
		void                copy(wchar_t(&dest)[DestSize], const wchar_t* const source);
		template<typename T>
		void                copy(T* dest, const T* const source, const uint32 byteCount);

		constexpr U8CharCode    encode(const char8_t ch);
		constexpr U8CharCode    encode(const char8_t(&ch)[2]);
		constexpr U8CharCode    encode(const char8_t(&ch)[3]);
		constexpr U8CharCode    encode(const char8_t(&ch)[4]);
		constexpr U8CharCode    encode(const char8_t* const string, const uint32 byteAt);
		StringU8                decode(const U8CharCode code);

		StringU8        convertWideStringToUTF8(const StringW& source);
		StringW         convertUTF8ToWideString(const StringU8& source);
		void            convertWideStringToString(const StringW& source, StringA& destination);
		void            convertWideStringToString(const std::wstring& source, std::string& destination);
		void            convertStringToWideString(const std::string& source, std::wstring& destination);

		void            convertStringAToStringW(const StringA& source, StringW& destination) noexcept;

		template <uint32 BufferSize>
		void            convertStackStringAToStackStringW(const StackStringA<BufferSize>& source, StackStringW<BufferSize>& destination) noexcept;

		void            excludeExtension(std::string& inoutText);

		template <typename T>
		void            tokenize(const std::basic_string<T>& inputString, const T delimiter, Vector<std::basic_string<T>>& outTokens);
		template <typename T>
		void            tokenize(const std::basic_string<T>& inputString, const Vector<T>& delimiters, Vector<std::basic_string<T>>& outTokens);

		template<typename INT, typename T>
		std::enable_if_t<std::is_integral_v<INT>, void>         toString(const INT i, MutableString<T>& outString);
		template<typename FLT, typename T>
		std::enable_if_t<std::is_floating_point_v<FLT>, void>   toString(const FLT f, MutableString<T>& outString);

		template<typename T>
		int32 stringToInt32(const StringReference<T>& string);
		template<typename T>
		int64 stringToInt64(const StringReference<T>& string);
		template<typename T>
		float stringToFloat(const StringReference<T>& string);
		template<typename T>
		double stringToDouble(const StringReference<T>& string);
	}
}


#include <MintContainer/Include/StringUtil.inl>


#endif // !_MINT_CONTAINER_STRING_UTIL_H_
