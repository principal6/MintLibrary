#pragma once


#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
	template<typename T>
	bool StringReference<T>::operator==(const StringReference<T>& rhs) const
	{
		return StringUtil::Equals(CString(), rhs.CString());
	}

	template<typename T>
	uint32 StringReference<T>::Length() const
	{
		return StringUtil::Length(CString());
	}

	template<typename T>
	uint32 StringReference<T>::CountChars() const
	{
		return StringUtil::CountChars(CString());
	}

	template<typename T>
	uint32 StringReference<T>::Find(const StringReference<T>& token, const uint32 offset) const
	{
		const uint32 sourceLength = Length();
		const uint32 tokenLength = token.Length();
		if (sourceLength < offset + tokenLength)
		{
			return kStringNPos;
		}

		const T* const sourceString = CString();
		const T* const tokenString = token.CString();
		uint32 result = kStringNPos;
		uint32 tokenResult = kStringNPos;
		uint32 tokenIter = 0;
		for (uint32 sourceIter = offset; sourceIter < sourceLength; ++sourceIter)
		{
			if (sourceString[sourceIter] == tokenString[tokenIter])
			{
				if (tokenIter == 0)
				{
					tokenResult = sourceIter;
				}

				++tokenIter;
				if (tokenIter == tokenLength)
				{
					result = tokenResult;
					break;
				}
			}
			else
			{
				tokenIter = 0;
				result = kStringNPos;
			}
		}
		return result;
	}

	template<typename T>
	inline uint32 StringReference<T>::RFind(const StringReference<T>& token, const uint32 offset) const
	{
		const uint32 stringLength = Length();
		const uint32 tokenLength = token.Length();
		if (stringLength < tokenLength)
		{
			return kStringNPos;
		}

		const uint32 stringStart = stringLength - tokenLength;
		if (stringStart < offset)
		{
			return kStringNPos;
		}
		const T* const string = CString();
		const T* const tokenString = token.CString();
		for (uint32 stringByteAt = stringStart - offset; stringByteAt != kUint32Max; --stringByteAt)
		{
			uint32 tokenByteAt = 0;
			while (tokenByteAt < tokenLength)
			{
				if (string[stringByteAt + tokenByteAt] != tokenString[tokenByteAt])
				{
					break;
				}
				++tokenByteAt;
			}

			if (tokenByteAt == tokenLength)
			{
				return stringByteAt;
			}
		}
		return kStringNPos;
	}

	template<typename T>
	bool StringReference<T>::Contains(const StringReference<T>& token, const uint32 offset) const
	{
		return Find(token, offset) != kStringNPos;
	}

	template<typename T>
	bool StringReference<T>::StartsWith(const StringReference<T>& token) const
	{
		return Find(token, 0) != kStringNPos;
	}
	
	template<typename T>
	bool StringReference<T>::EndsWith(const StringReference<T>& token) const
	{
		return Find(token, Length() - token.Length()) != kStringNPos;
	}

	template<typename T>
	uint64 StringReference<T>::ComputeHash() const
	{
		return mint::ComputeHash(CString());
	}


#pragma region MutableString
	template<typename T>
	inline MutableString<T>& mint::MutableString<T>::operator=(const StringReference<T>& rhs)
	{
		return Assign(rhs);
	}

	template<typename T>
	inline MutableString<T>& mint::MutableString<T>::operator=(StringReference<T>&& rhs)
	{
		return Assign(rhs);
	}

	template<typename T>
	inline MutableString<T>& mint::MutableString<T>::operator+=(const StringReference<T>& rhs)
	{
		return Append(rhs);
	}

	template<typename T>
	inline MutableString<T>& mint::MutableString<T>::operator+=(const T rhs)
	{
		return Append(rhs);
	}
#pragma endregion
}
