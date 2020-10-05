#pragma once

#include <stdafx.h>
#include <Container/UniqueString.h>

#include <Container/StringUtil.h>


namespace fs
{
	FS_INLINE const bool UniqueStringA::operator==(const UniqueStringA& rhs) const noexcept
	{
		return (_index == rhs._index);
	}

	FS_INLINE const bool UniqueStringA::operator!=(const UniqueStringA& rhs) const noexcept
	{
		return !(*this == rhs);
	}


	FS_INLINE const UniqueStringA UniqueStringPoolA::getString(const uint32 index) const noexcept
	{
		if (_count <= index)
		{
			return UniqueStringA::kInvalidUniqueString;
		}
		return UniqueStringA(this, index);
	}

	FS_INLINE const char* UniqueStringPoolA::getRawString(const UniqueStringA& uniqueString) const noexcept
	{
		if (uniqueString == UniqueStringA::kInvalidUniqueString)
		{
			return nullptr;
		}
		return &_rawMemory[_offsetArray[uniqueString._index]];;
	}
}
