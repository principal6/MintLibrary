#pragma once

#include <stdafx.h>
#include <FsContainer/Include/UniqueString.h>

#include <FsContainer/Include/StringUtil.h>


namespace fs
{
	inline UniqueStringAId::UniqueStringAId()
		: _rawId{ UniqueStringAId::kInvalidRawId }
	{
		__noop;
	}

	inline UniqueStringAId::UniqueStringAId(const uint32 newRawId)
		: _rawId{ newRawId }
	{
		__noop;
	}

	FS_INLINE const bool UniqueStringAId::operator==(const UniqueStringAId& rhs) const noexcept
	{
		return _rawId == rhs._rawId;
	}

	FS_INLINE const bool UniqueStringAId::operator!=(const UniqueStringAId& rhs) const noexcept
	{
		return _rawId != rhs._rawId;
	}


	FS_INLINE const bool UniqueStringA::operator==(const UniqueStringA& rhs) const noexcept
	{
		return _id == rhs._id;
	}

	FS_INLINE const bool UniqueStringA::operator!=(const UniqueStringA& rhs) const noexcept
	{
		return _id != rhs._id;
	}

	FS_INLINE const char* UniqueStringA::c_str() const noexcept
	{
		return _pool.getRawString(_id);
	}

#if defined FS_UNIQUE_STRING_EXPOSE_ID
	FS_INLINE const UniqueStringAId UniqueStringA::getId() const noexcept
	{
		return _id;
	}
#endif


	FS_INLINE const bool UniqueStringPoolA::isValid(const UniqueStringAId id) const noexcept
	{
		return (id._rawId < _uniqueStringCount);
	}

	FS_INLINE const char* UniqueStringPoolA::getRawString(const UniqueStringAId id) const noexcept
	{
		if (isValid(id) == false)
		{
			return nullptr;
		}
		return &_rawMemory[_offsetArray[id._rawId]];;
	}
}
