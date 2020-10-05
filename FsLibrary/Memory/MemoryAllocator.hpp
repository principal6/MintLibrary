#pragma once

#include <stdafx.h>
#include <Memory\MemoryAllocator.h>


namespace fs
{
	template <class T>
	inline void MemoryAccessor::setMemory(const typename std::enable_if<std::is_literal_type<T>::value, T>::type value, const uint32 byteOffset)
	{
		setMemory(reinterpret_cast<const byte*>(&value), sizeof(value), byteOffset);
	}


	FS_INLINE const uint32 MemoryAllocator::convertBlockUnitToByteUnit(const uint32 blockUnit) const noexcept
	{
		return static_cast<uint32>(static_cast<uint64>(blockUnit) * kBlockSize);
	}

	FS_INLINE const uint32 MemoryAllocator::calculateBlockCount(const uint32 byteSize) const noexcept
	{
		const uint32 roundUp = (0 < (byteSize % kBlockSize)) ? 1 : 0;
		return static_cast<uint32>(static_cast<uint64>(byteSize) / kBlockSize + roundUp);
	}
}
