#pragma once


#include <MintContainer/Include/StringBase.h>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
	template <typename T>
	uint32 StringBase<T>::length() const
	{
		return StringUtil::length(c_str());
	}

	template <typename T>
	uint32 StringBase<T>::find(const StringBase<T>& token, const uint32 offset) const
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
	bool StringBase<T>::contains(const StringBase<T>& token, const uint32 offset) const
	{
		return find(token, offset) != kStringNPos;
	}

	template <typename T>
	uint64 StringBase<T>::computeHash() const
	{
		return mint::computeHash(c_str());
	}


	template <typename T>
	bool operator==(const StringBase<T>& lhs, const StringBase<T>& rhs)
	{
		return StringUtil::compare(lhs.c_str(), rhs.c_str());

		//if (lhs.isMutable() || rhs.isMutable())
		//{
		//	return StringUtil::compare(lhs.c_str(), rhs.c_str());
		//}
		////TODO
		//return false;
	}
}
