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
		: _bucketId{ MemoryBucketId::kInvalidMemoryBucketId }, _memoryAllocator{ nullptr }
	{
		__noop;
	}

	MemoryAccessor::MemoryAccessor(const MemoryAccessor& rhs)
		: _bucketId{ rhs._bucketId }, _memoryAllocator{ rhs._memoryAllocator }
	{
		_memoryAllocator->increaseReferenceXXX(_bucketId);
	}

	MemoryAccessor::MemoryAccessor(MemoryAccessor&& rhs) noexcept
		: _bucketId{ rhs._bucketId }, _memoryAllocator{ rhs._memoryAllocator }
	{
		rhs._bucketId = MemoryBucketId::kInvalidMemoryBucketId;
		rhs._memoryAllocator = nullptr;
	}

	MemoryAccessor::~MemoryAccessor()
	{
		if (_memoryAllocator != nullptr)
		{
			_memoryAllocator->decreaseReferenceXXX(_bucketId, *this);
		}
	}

	MemoryAccessor::MemoryAccessor(const MemoryBucketId bucketId, MemoryAllocator* const memoryAllocator)
		: _bucketId{ bucketId }, _memoryAllocator{ memoryAllocator }
	{
		__noop;
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

				rhs._bucketId = MemoryBucketId::kInvalidMemoryBucketId;
				rhs._memoryAllocator = nullptr;
			}
		}
		return *this;
	}

	const bool MemoryAccessor::isValid() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->isValid(_bucketId) : false;
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
		}
	}

	const byte* MemoryAccessor::getMemory() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->getMemoryXXX(_bucketId) : nullptr;
	}

	const uint32 MemoryAccessor::getByteCapacity() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->getByteCapacity(_bucketId) : 0;
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

		std::scoped_lock<std::mutex> scopedLock(_mutex);

		MemoryBucket newBucket;
		newBucket._id.assignIdXXX();
		newBucket._blockOffset = availableBlockOffset;
		newBucket._blockCount = newBlockCount;
		newBucket._referenceCount = 1;
#if defined FS_MEMORY_USE_IS_SHARED_FLAG
		newBucket._isShared = isShared;
#endif
		_bucketMap.insert(std::make_pair(newBucket._id.getRawIdXXX(), newBucket)); // 절대 실패하면 안 된다.

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
			auto found = _bucketMap.find(memoryAccessor._bucketId.getRawIdXXX());
			if (found != _bucketMap.end())
			{

				MemoryBucket& memoryBucket = found->second;
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
		auto found = _bucketMap.find(memoryAccessor._bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			{
				const MemoryBucket& bucket = found->second;
				for (uint32 blockIter = 0; blockIter < bucket._blockCount; blockIter++)
				{
					_blockInUseArray.set(bucket._blockOffset + blockIter, false);
				}
			}

			_bucketMap.erase(found->first); // 절대 실패하면 안 된다.

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

	const bool MemoryAllocator::isValid(const MemoryBucketId bucketId) const noexcept
	{
		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		return (found != _bucketMap.end());
	}

	void MemoryAllocator::setMemoryXXX(const MemoryBucketId bucketId, const byte* const source, const uint32 byteCount, const uint32 byteOffset)
	{
		if (source == nullptr)
		{
			return;
		}

		std::scoped_lock<std::mutex> scopedLock(_mutex);

		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			MemoryBucket& memoryBucket = found->second;
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

	const byte* MemoryAllocator::getMemoryXXX(const MemoryBucketId bucketId) const
	{
		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			return _rawMemory + convertBlockUnitToByteUnit(found->second._blockOffset);
		}
		return nullptr;
	}

	const uint32 MemoryAllocator::getByteCapacity(const MemoryBucketId bucketId) const
	{
		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			return convertBlockUnitToByteUnit(found->second._blockCount);
		}
		return 0;
	}

	void MemoryAllocator::increaseReferenceXXX(const MemoryBucketId bucketId)
	{
		std::scoped_lock<std::mutex> scopedLock(_mutex);

		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			++found->second._referenceCount;
		}
	}
	
	void MemoryAllocator::decreaseReferenceXXX(const MemoryBucketId bucketId, MemoryAccessor& memoryAccessor)
	{
		std::scoped_lock<std::mutex> scopedLock(_mutex);
		
		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			--found->second._referenceCount;

			if (found->second._referenceCount == 0)
			{
				deallocate(memoryAccessor);
			}
		}
	}
}
