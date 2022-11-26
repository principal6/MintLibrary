#pragma once


#include <MintContainer/Include/StringReference.h>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
	template <typename T>
	bool StringReference<T>::operator==(const StringReference<T>& rhs) const
	{
		return StringUtil::compare(c_str(), rhs.c_str());
	}

	template <typename T>
	uint32 StringReference<T>::length() const
	{
		return StringUtil::countChars(c_str());
	}
	
	template <typename T>
	uint32 StringReference<T>::countBytes() const
	{
		return StringUtil::countChars(c_str());
	}
	
	template <typename T>
	uint32 StringReference<T>::countChars() const
	{
		return StringUtil::countChars(c_str());
	}

	template <typename T>
	uint32 StringReference<T>::find(const StringReference<T>& token, const uint32 offset) const
	{
		const uint32 sourceLength = length();
		const uint32 tokenLength = token.length();
		if (sourceLength < offset + tokenLength)
		{
			return kStringNPos;
		}

		const T* const sourceString = c_str();
		const T* const tokenString = token.c_str();
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
	
	template <typename T>
	inline uint32 StringReference<T>::rfind(const StringReference<T>& token, const uint32 offset) const
	{
		const uint32 stringByteCount = countBytes();
		const uint32 tokenByteCount = token.countBytes();
		if (stringByteCount < tokenByteCount)
		{
			return kStringNPos;
		}

		const uint32 stringByteStart = stringByteCount - tokenByteCount;
		if (stringByteStart < offset)
		{
			return kStringNPos;
		}
		const T* const string = c_str();
		const T* const tokenString = token.c_str();
		for (uint32 stringByteAt = stringByteStart - offset; stringByteAt != kUint32Max; --stringByteAt)
		{
			uint32 tokenByteAt = 0;
			while (tokenByteAt < tokenByteCount)
			{
				if (string[stringByteAt + tokenByteAt] != tokenString[tokenByteAt])
				{
					break;
				}
				++tokenByteAt;
			}

			if (tokenByteAt == tokenByteCount)
			{
				return stringByteAt;
			}
		}
		return kStringNPos;
	}

	template <typename T>
	bool StringReference<T>::contains(const StringReference<T>& token, const uint32 offset) const
	{
		return find(token, offset) != kStringNPos;
	}

	template <typename T>
	uint64 StringReference<T>::computeHash() const
	{
		return mint::computeHash(c_str());
	}
}
