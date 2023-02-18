#pragma once


#ifndef _MINT_CONTAINER_STRING_VIEW_H_
#define _MINT_CONTAINER_STRING_VIEW_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template<typename T>
	class String;

	template<typename T, uint32 BufferSize>
	class StackString;


	template<typename T>
	class StringView
	{
	public:
		StringView(const T* const string);
		StringView(const String<T>& string);
		template <uint32 BufferSize>
		StringView(const StackString<T, BufferSize>& string);
		~StringView() = default;

	public:
		bool operator==(const StringView& rhs) const noexcept;
		bool operator!=(const StringView& rhs) const noexcept;
		const T& operator[](const uint32 at) const noexcept;
		const T* CString() const noexcept;
		uint32 Length() const noexcept { return _length; }
		bool IsEmpty() const noexcept { return _length == 0; }

	private:
		const T* const _rawString;
		const uint32 _length;
	};


	using StringViewA = StringView<char>;
	using StringViewW = StringView<wchar_t>;
	using StringViewU8 = StringView<char8_t>;
}


#endif // !_MINT_CONTAINER_STRING_VIEW_H_
