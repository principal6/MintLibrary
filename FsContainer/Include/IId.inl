#pragma once


namespace fs
{
	FS_INLINE const bool			IId::operator==(const IId& rhs) const noexcept
	{
		return _rawId == rhs._rawId;
	}

	FS_INLINE const bool			IId::operator!=(const IId& rhs) const noexcept
	{
		return _rawId != rhs._rawId;
	}

	FS_INLINE const bool			IId::operator<(const IId& rhs) const noexcept
	{
		return _rawId < rhs._rawId;
	}

	FS_INLINE const bool			IId::operator>(const IId& rhs) const noexcept
	{
		return _rawId > rhs._rawId;
	}

	FS_INLINE const bool			IId::isValid() const noexcept
	{
		return _rawId != kInvalidRawId;
	}
}
