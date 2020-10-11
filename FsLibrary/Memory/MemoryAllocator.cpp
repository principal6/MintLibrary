#include <stdafx.h>
#include <Memory\MemoryAllocator.h>
#include <Memory\MemoryAllocator.hpp>

#include <Container\BitVector.hpp>
#include <Container\StringUtil.h>


namespace fs
{
	std::atomic<MemoryBucketRawIdType>	MemoryBucketId::_lastRawId{ MemoryBucketId::kInvalidMemoryBucketRawId };
	const MemoryBucketId				MemoryBucketId::kInvalidMemoryBucketId;


	MemoryAccessor::MemoryAccessor()
		: _bucketId{ MemoryBucketId::kInvalidMemoryBucketId }
		, _memoryAllocator{ nullptr }
#if defined FS_DEBUG
		, _rawMemoryView{ nullptr }
#endif
	{
		__noop;
	}

	MemoryAccessor::MemoryAccessor(const MemoryBucketId bucketId, MemoryAllocator* const memoryAllocator)
		: _bucketId{ bucketId }
		, _memoryAllocator{ memoryAllocator }
#if defined FS_DEBUG
		, _rawMemoryView{ _memoryAllocator->getMemoryXXX(_bucketId) }
#endif
	{
		__noop;
	}

	MemoryAccessor::MemoryAccessor(const MemoryAccessor& rhs)
		: _bucketId{ rhs._bucketId }
		, _memoryAllocator{ rhs._memoryAllocator }
#if defined FS_DEBUG
		, _rawMemoryView{ nullptr }
#endif
	{
		_memoryAllocator->increaseReferenceXXX(_bucketId);
	}

	MemoryAccessor::MemoryAccessor(MemoryAccessor&& rhs) noexcept
		: _bucketId{ rhs._bucketId }
		, _memoryAllocator{ rhs._memoryAllocator }
#if defined FS_DEBUG
		, _rawMemoryView{ rhs._rawMemoryView }
#endif
	{
		rhs._bucketId = MemoryBucketId::kInvalidMemoryBucketId;
		rhs._memoryAllocator = nullptr;
#if defined FS_DEBUG
		rhs._rawMemoryView = nullptr;
#endif
	}

	MemoryAccessor::~MemoryAccessor()
	{
		if (_memoryAllocator != nullptr)
		{
			_memoryAllocator->decreaseReferenceXXX(_bucketId, *this);
		}
	}

	MemoryAccessor& MemoryAccessor::operator=(const MemoryAccessor& rhs)
	{
		if (&rhs != this)
		{
			if (rhs._memoryAllocator != nullptr)
			{
				if (_memoryAllocator != nullptr)
				{
					_memoryAllocator->decreaseReferenceXXX(_bucketId, *this);
				}

				_bucketId = rhs._bucketId;
				_memoryAllocator = rhs._memoryAllocator;
#if defined FS_DEBUG
				_rawMemoryView = rhs._rawMemoryView;
#endif

				_memoryAllocator->increaseReferenceXXX(_bucketId);
			}
		}
		return *this;
	}

	MemoryAccessor& MemoryAccessor::operator=(MemoryAccessor&& rhs) noexcept
	{
		if (&rhs != this)
		{
			if (rhs._memoryAllocator != nullptr)
			{
				if (_memoryAllocator != nullptr)
				{
					_memoryAllocator->decreaseReferenceXXX(_bucketId, *this);
				}

				_bucketId = rhs._bucketId;
				_memoryAllocator = rhs._memoryAllocator;
#if defined FS_DEBUG
				_rawMemoryView = rhs._rawMemoryView;
#endif

				rhs._bucketId = MemoryBucketId::kInvalidMemoryBucketId;
				rhs._memoryAllocator = nullptr;
#if defined FS_DEBUG
				rhs._rawMemoryView = nullptr;
#endif
			}
		}
		return *this;
	}

	void MemoryAccessor::setMemory(const char* const source, const uint32 byteOffset)
	{
		setMemory(reinterpret_cast<const byte*>(source), fs::StringUtil::strlen(source), byteOffset);
	}

	void MemoryAccessor::setMemory(const byte* const source, const uint32 byteCount, const uint32 byteOffset)
	{
		if (_memoryAllocator != nullptr)
		{
			_memoryAllocator->setMemoryXXX(_bucketId, source, byteCount, byteOffset);
#if defined FS_DEBUG
			_rawMemoryView = _memoryAllocator->getMemoryXXX(_bucketId);
#endif
		}
	}


	MemoryAllocator::MemoryAllocator()
		: kBlockSize{ kDefaultBlockSize }
		, _rawCapacity{ 0 }
		, _rawMemory{ nullptr }
	{
		reserve(kDefaultInitialBlockCapacity);
	}

	MemoryAllocator::~MemoryAllocator()
	{
		cleanUpRawMemory();
	}

#if defined FS_MEMORY_USE_IS_SHARED_FLAG
	MemoryAccessor MemoryAllocator::allocate(const uint32 newByteSize, const bool isShared)
#else
	MemoryAccessor MemoryAllocator::allocate(const uint32 newByteSize)
#endif
	{
		const uint32 newBlockCount = calculateBlockCount(newByteSize);
		uint32 availableBlockOffset = getAvailableBlockOffsetXXX(newBlockCount);
		if (availableBlockOffset == kUint32Max)
		{
			reserve(calculateBlockCount(_rawCapacity) * 2);
			availableBlockOffset = getAvailableBlockOffsetXXX(newBlockCount);
		}

		uint32 availableBucketIndex = getAvailableBucketIndexXXX();
		if (availableBucketIndex == kUint32Max)
		{
			availableBucketIndex = static_cast<uint32>(_bucketArray.size());
		}

		std::scoped_lock<std::mutex> scopedLock(_mutex);

		MemoryBucket newBucket;
		newBucket._id.assignIdXXX();
		newBucket._id.assignBucketIndexXXX(availableBucketIndex);
		newBucket._blockOffset = availableBlockOffset;
		newBucket._blockCount = newBlockCount;
		newBucket._referenceCount = 1;
#if defined FS_MEMORY_USE_IS_SHARED_FLAG
		newBucket._isShared = isShared;
#endif
		if (availableBucketIndex == static_cast<uint32>(_bucketArray.size()))
		{
			_bucketArray.emplace_back(newBucket);
			_bucketInUseArray.push_back(true);
		}
		else
		{
			_bucketArray[availableBucketIndex] = newBucket;
			_bucketInUseArray.set(availableBucketIndex, true);
		}

		for (uint32 blockIter = 0; blockIter < newBucket._blockCount; blockIter++)
		{
			_blockInUseArray.set(newBucket._blockOffset + blockIter, true);
		}

		return MemoryAccessor(newBucket._id, this);
	}

	MemoryAccessor& MemoryAllocator::reallocate(MemoryAccessor& memoryAccessor, const uint32 newByteSize, const bool keepData)
	{
		const uint32 newBlockCount = calculateBlockCount(newByteSize);
		uint32 availableBlockOffset = getAvailableBlockOffsetXXX(newBlockCount);
		if (availableBlockOffset == kUint32Max)
		{
			reserve(calculateBlockCount(_rawCapacity) * 2);
			availableBlockOffset = getAvailableBlockOffsetXXX(newBlockCount);
		}

		{
			std::scoped_lock<std::mutex> scopedLock(_mutex);
			
			if (memoryAccessor.isValid() == true)
			{
				MemoryBucket& memoryBucket = getMemoryBucketXXX(memoryAccessor._bucketId);
				const uint32 oldBlockOffset = memoryBucket._blockOffset;
				const uint32 oldBlockCount = memoryBucket._blockCount;
				memoryBucket._blockOffset = availableBlockOffset;
				memoryBucket._blockCount = newBlockCount;

				for (uint32 blockIter = 0; blockIter < oldBlockCount; blockIter++)
				{
					_blockInUseArray.set(oldBlockOffset + blockIter, false);
				}

				for (uint32 blockIter = 0; blockIter < memoryBucket._blockCount; blockIter++)
				{
					_blockInUseArray.set(memoryBucket._blockOffset + blockIter, true);
				}

				if (keepData == true)
				{
					memcpy(_rawMemory + convertBlockUnitToByteUnit(memoryBucket._blockOffset), _rawMemory + convertBlockUnitToByteUnit(oldBlockOffset), sizeof(byte) * convertBlockUnitToByteUnit(oldBlockCount));
				}

				return memoryAccessor;
			}
		}
		
		memoryAccessor = allocate(newByteSize);
		return memoryAccessor;
	}

	void MemoryAllocator::deallocate(MemoryAccessor& memoryAccessor)
	{
		MemoryBucket& memoryBucket = getMemoryBucketXXX(memoryAccessor._bucketId);
		if (memoryBucket._id == memoryAccessor._bucketId)
		{
			{
				for (uint32 blockIter = 0; blockIter < memoryBucket._blockCount; blockIter++)
				{
					_blockInUseArray.set(memoryBucket._blockOffset + blockIter, false);
				}
			}

			_bucketInUseArray.set(memoryBucket._id.getBucketIndexXXX(), false);
			memoryBucket._id = MemoryBucketId::kInvalidMemoryBucketId;

			memoryAccessor._bucketId = MemoryBucketId::kInvalidMemoryBucketId;
			memoryAccessor._memoryAllocator = nullptr;
		}
	}

	const uint32 MemoryAllocator::getAvailableBlockOffsetXXX(const uint32 blockCount) const noexcept
	{
		const uint32 bitCount = _blockInUseArray.bitCount();
		uint32 successiveBitCount = 0;
		for (uint32 bitAt = 0; bitAt < bitCount; bitAt++)
		{
			if (_blockInUseArray.get(bitAt) == false)
			{
				++successiveBitCount;
				if (successiveBitCount == blockCount)
				{
					return (bitAt + 1) - successiveBitCount;
				}
			}
			else
			{
				successiveBitCount = 0;
			}
		}
		return kUint32Max;
	}

	const uint32 MemoryAllocator::getAvailableBucketIndexXXX() const noexcept
	{
		const uint32 bitCount = _bucketInUseArray.bitCount();
		for (uint32 bitAt = 0; bitAt < bitCount; bitAt++)
		{
			if (_bucketInUseArray.get(bitAt) == false)
			{
				return bitAt;
			}
		}
		return kUint32Max;
	}

	void MemoryAllocator::reserve(const uint32 blockCapacity)
	{
		const uint32 rawCapacity = blockCapacity * kBlockSize;

		std::scoped_lock<std::mutex> scopedLock(_mutex);
		
		if (_rawCapacity < rawCapacity)
		{
			{
				byte* temp = FS_NEW_ARRAY(byte, _rawCapacity);
				memcpy(temp, _rawMemory, sizeof(byte) * _rawCapacity);
				FS_DELETE_ARRAY(_rawMemory);

				_rawMemory = FS_NEW_ARRAY(byte, rawCapacity);
				memcpy(_rawMemory, temp, sizeof(byte) * _rawCapacity);
				FS_DELETE_ARRAY(temp);
			}

			_rawCapacity = rawCapacity;
			_blockInUseArray.resizeBitCount(blockCapacity);
		}
	}

	void MemoryAllocator::cleanUpRawMemory()
	{
		FS_DELETE_ARRAY(_rawMemory);
	}

	void MemoryAllocator::setMemoryXXX(const MemoryBucketId bucketId, const byte* const source, const uint32 byteCount, const uint32 byteOffset)
	{
		if (source == nullptr)
		{
			return;
		}

		std::scoped_lock<std::mutex> scopedLock(_mutex);

		const MemoryBucket& memoryBucket = getMemoryBucketXXX(bucketId);
		if (memoryBucket._id == bucketId)
		{
			const uint32 destinationByteOffset = convertBlockUnitToByteUnit(memoryBucket._blockOffset);
			const uint32 blockByteCapacity = static_cast<uint32>(sizeof(byte) * convertBlockUnitToByteUnit(memoryBucket._blockCount));
			if (blockByteCapacity <= byteOffset)
			{
				return;
			}

			const uint32 copyByteSize = fs::min(blockByteCapacity - byteOffset, byteCount);
			if (copyByteSize == 0)
			{
				return;
			}

			memcpy(_rawMemory + destinationByteOffset + byteOffset, source, copyByteSize);
		}
	}

	void MemoryAllocator::increaseReferenceXXX(const MemoryBucketId bucketId)
	{
		std::scoped_lock<std::mutex> scopedLock(_mutex);

		MemoryBucket& memoryBucket = getMemoryBucketXXX(bucketId);
		if (memoryBucket._id == bucketId)
		{
			++memoryBucket._referenceCount;
		}
	}
	
	void MemoryAllocator::decreaseReferenceXXX(const MemoryBucketId bucketId, MemoryAccessor& memoryAccessor)
	{
		std::scoped_lock<std::mutex> scopedLock(_mutex);
		
		MemoryBucket& memoryBucket = getMemoryBucketXXX(bucketId);
		if (memoryBucket._id == bucketId)
		{
			--memoryBucket._referenceCount;

			if (memoryBucket._referenceCount == 0)
			{
				deallocate(memoryAccessor);
			}
		}
	}
}
