#include <stdafx.h>
#include <Memory\MemoryAllocator.h>

#include <Container\BitVector.hpp>


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

	const bool MemoryAccessor::isValid() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->isValid(_bucketId) : false;
	}

	void MemoryAccessor::setMemory(const byte* const content)
	{
		if (_memoryAllocator != nullptr)
		{
			_memoryAllocator->setMemoryXXX(_bucketId, content);
		}
	}

	const byte* MemoryAccessor::getMemory() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->getMemoryXXX(_bucketId) : nullptr;
	}

	const uint32 MemoryAccessor::getByteSize() const noexcept
	{
		return (_memoryAllocator != nullptr) ? _memoryAllocator->getByteSize(_bucketId) : 0;
	}


	MemoryAllocator::MemoryAllocator()
		: kBlockSize{ kDefaultBlockSize }
		, _rawCapacity{ 0 }
		, _rawMemory{ nullptr }
		, _nextAvailableBlockIndex{ 0 }
	{
		reserve(kDefaultInitialBlockCapacity);
	}

	MemoryAllocator::~MemoryAllocator()
	{
		cleanUpRawMemory();
	}

#if defined FS_MEMORY_USE_IS_SHARED_FLAG
	MemoryAccessor MemoryAllocator::allocate(const uint32 byteSize, const bool isShared)
#else
	MemoryAccessor MemoryAllocator::allocate(const uint32 byteSize)
#endif
	{
		const uint32 newBlockCount = byteSize / kBlockSize;
		if (canAllocate(newBlockCount) == false)
		{
			reserve(_rawCapacity * 2);
		}

		std::scoped_lock<std::mutex> scopedLock(_mutex);

		MemoryBucket newBucket;
		newBucket._id.assignIdXXX();
		newBucket._byteSize = byteSize;
		newBucket._blockOffset = _nextAvailableBlockIndex;
		newBucket._blockCount = newBlockCount;
		newBucket._referenceCount = 1;
#if defined FS_MEMORY_USE_IS_SHARED_FLAG
		newBucket._isShared = isShared;
#endif
		_bucketMap.insert(std::make_pair(newBucket._id.getRawIdXXX(), newBucket)); // 절대 실패하면 안 된다.

		// ===
		_nextAvailableBlockIndex = newBucket._blockOffset + newBucket._blockCount;
		for (uint32 blockIter = 0; blockIter < newBucket._blockCount; blockIter++)
		{
			_blockInUseArray.set(newBucket._blockOffset + blockIter, true);
		}
		// ===

		return MemoryAccessor(newBucket._id, this);
	}

	void MemoryAllocator::deallocate(MemoryAccessor& memoryAccessor)
	{
		auto found = _bucketMap.find(memoryAccessor._bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			// ===
			MemoryBucket& bucket = found->second;
			for (uint32 blockIter = 0; blockIter < bucket._blockCount; blockIter++)
			{
				_blockInUseArray.set(bucket._blockOffset + blockIter, false);
			}
			// ===

			_bucketMap.erase(found->first); // 절대 실패하면 안 된다.

			memoryAccessor._bucketId = MemoryBucketId::kInvalidMemoryBucketId;
			memoryAccessor._memoryAllocator = nullptr;
		}
	}

	FS_INLINE const bool MemoryAllocator::canAllocate(const uint32 blockCount) const noexcept
	{
		return (_nextAvailableBlockIndex + blockCount <= _blockInUseArray.size());
	}

	void MemoryAllocator::reserve(const uint32 blockCapacity)
	{
		const uint32 rawCapacity = blockCapacity * kBlockSize;

		if (_rawCapacity < rawCapacity)
		{
			std::scoped_lock<std::mutex> scopedLock(_mutex);

			{
				byte* temp = FS_NEW_ARRAY(byte, _rawCapacity);
				memcpy(temp, _rawMemory, sizeof(byte) * _rawCapacity);
				FS_DELETE_ARRAY(_rawMemory);

				_rawMemory = FS_NEW_ARRAY(byte, rawCapacity);
				memcpy(_rawMemory, temp, sizeof(byte) * _rawCapacity);
				FS_DELETE_ARRAY(temp);
			}

			_rawCapacity = rawCapacity;
			_blockInUseArray.resize(blockCapacity);
		}
	}

	void MemoryAllocator::cleanUpRawMemory()
	{
		FS_DELETE_ARRAY(_rawMemory);
	}

	FS_INLINE const size_t MemoryAllocator::convertBlockUnitToByteUnit(const uint32 blockUnit) const noexcept
	{
		return static_cast<size_t>(blockUnit) * kBlockSize;
	}

	const bool MemoryAllocator::isValid(const MemoryBucketId bucketId) const noexcept
	{
		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		return (found != _bucketMap.end());
	}

	void MemoryAllocator::setMemoryXXX(const MemoryBucketId bucketId, const byte* const content)
	{
		if (content == nullptr)
		{
			return;
		}

		std::scoped_lock<std::mutex> scopedLock(_mutex);

		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			memcpy(_rawMemory + convertBlockUnitToByteUnit(found->second._blockOffset), content, sizeof(byte) * convertBlockUnitToByteUnit(found->second._blockCount));
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
	
	const uint32 MemoryAllocator::getByteSize(const MemoryBucketId bucketId) const
	{
		auto found = _bucketMap.find(bucketId.getRawIdXXX());
		if (found != _bucketMap.end())
		{
			return found->second._byteSize;
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
