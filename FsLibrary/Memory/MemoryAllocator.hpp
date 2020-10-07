#pragma once

#include <stdafx.h>
#include <Memory\MemoryAllocator.h>


namespace fs
{
	FS_INLINE const bool MemoryBucketId::operator==(const MemoryBucketId& rhs) const noexcept
	{
		return (_rawId == rhs._rawId) && (_bucketIndex == rhs._bucketIndex);
	}


	FS_INLINE void MemoryBucketId::assignIdXXX() noexcept
	{
		++_lastRawId;
		_rawId = _lastRawId;
	}

	FS_INLINE void MemoryBucketId::assignBucketIndexXXX(const uint32 bucketIndex) noexcept
	{
		_bucketIndex = bucketIndex;
	}

	FS_INLINE const uint32 MemoryBucketId::getBucketIndexXXX() const noexcept
	{
		return _bucketIndex;
	}


	FS_INLINE const bool MemoryAccessor::isValid() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->isValid(_bucketId) : false;
	}

	template <class T>
	inline void MemoryAccessor::setMemory(const typename std::enable_if<std::is_literal_type<T>::value, T>::type value, const uint32 byteOffset)
	{
		setMemory(reinterpret_cast<const byte*>(&value), sizeof(value), byteOffset);
	}

	FS_INLINE const byte* MemoryAccessor::getMemory() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->getMemoryXXX(_bucketId) : nullptr;
	}

	FS_INLINE const uint32 MemoryAccessor::getByteCapacity() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->getByteCapacity(_bucketId) : 0;
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

	FS_INLINE const bool MemoryAllocator::isValid(const MemoryBucketId bucketId) const noexcept
	{
		return (_bucketArray[bucketId.getBucketIndexXXX()]._id == bucketId);
	}

	FS_INLINE const byte* MemoryAllocator::getMemoryXXX(const MemoryBucketId bucketId) const
	{
		const MemoryBucket& memoryBucket = getMemoryBucket(bucketId);
		if (memoryBucket._id == bucketId)
		{
			return _rawMemory + convertBlockUnitToByteUnit(memoryBucket._blockOffset);
		}
		return nullptr;
	}

	FS_INLINE const uint32 MemoryAllocator::getByteCapacity(const MemoryBucketId bucketId) const
	{
		const MemoryBucket& memoryBucket = getMemoryBucket(bucketId);
		if (memoryBucket._id == bucketId)
		{
			return convertBlockUnitToByteUnit(memoryBucket._blockCount);
		}
		return 0;
	}

	FS_INLINE const MemoryAllocator::MemoryBucket& MemoryAllocator::getMemoryBucket(const MemoryBucketId bucketId) const
	{
		return _bucketArray[bucketId.getBucketIndexXXX()];
	}

	FS_INLINE MemoryAllocator::MemoryBucket& MemoryAllocator::getMemoryBucketXXX(const MemoryBucketId bucketId)
	{
		return _bucketArray[bucketId.getBucketIndexXXX()];
	}

}
