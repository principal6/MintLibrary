#pragma once


#ifndef _MINT_CONTAINER_STRING_VIEW_HPP_
#define _MINT_CONTAINER_STRING_VIEW_HPP_


#include <MintContainer/Include/StringView.h>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StackString.hpp>


namespace mint
{
	template<typename T>
	inline StringView<T>::StringView(const T* const string)
		: _rawString{ string }
		, _length{ StringUtil::Length(string) }
	{
		__noop;
	}

	template<typename T>
	inline StringView<T>::StringView(const String<T>& string)
		: _rawString{ string.CString() }
		, _length{ string.Length() }
	{
		__noop;
	}

	template<typename T>
	template<uint32 BufferSize>
	inline StringView<T>::StringView(const StackString<T, BufferSize>& string)
		: _rawString{ string.CString() }
		, _length{ string.Length() }
	{
		__noop;
	}

	template<typename T>
	bool StringView<T>::operator==(const StringView& rhs) const noexcept
	{
		return StringUtil::Equals(_rawString, rhs._rawString);
	}

	template<typename T>
	bool StringView<T>::operator!=(const StringView& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	template<typename T>
	const T& StringView<T>::operator[](const uint32 at) const noexcept
	{
		return _rawString[at];
	}

	template<typename T>
	const T* StringView<T>::CString() const noexcept
	{
		return _rawString;
	}
}


#endif // !_MINT_CONTAINER_STRING_VIEW_HPP_
