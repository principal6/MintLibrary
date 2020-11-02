#pragma once


#include <stdafx.h>
#include <Memory/MemoryAllocator2.h>


namespace fs
{
#pragma region memoryAccessor
	template<typename T>
	inline MemoryAccessor2<T>::MemoryAccessor2(const MemoryAccessor2& rhs)
		: _memoryAllocator{ rhs._memoryAllocator }
		, _id{ rhs._id }
		, _blockOffset{ rhs._blockOffset }
#if defined FS_DEBUG
		, _rawPointerForDebug{ rhs._rawPointerForDebug }
#endif
	{
		_memoryAllocator->increaseReferenceXXX(*this);
	}

	template<typename T>
	inline MemoryAccessor2<T>::MemoryAccessor2(MemoryAccessor2&& rhs)
		: _memoryAllocator{ rhs._memoryAllocator }
		, _id{ rhs._id }
		, _blockOffset{ rhs._blockOffset }
#if defined FS_DEBUG
		, _rawPointerForDebug{ rhs._rawPointerForDebug }
#endif
	{
		rhs._memoryAllocator = nullptr;
		rhs._id = kMemoryBlockIdInvalid;
		rhs._blockOffset = 0;
#if defined FS_DEBUG
		rhs._rawPointerForDebug = nullptr;
#endif
	}

	template<typename T>
	inline MemoryAccessor2<T>::~MemoryAccessor2()
	{
		if (_memoryAllocator != nullptr)
		{
			_memoryAllocator->decreaseReferenceXXX(*this);
		}
	}

	template<typename T>
	inline MemoryAccessor2<T>& MemoryAccessor2<T>::operator=(const MemoryAccessor2& rhs)
	{
		if (this != &rhs)
		{
			if (_memoryAllocator != nullptr)
			{
				_memoryAllocator->decreaseReferenceXXX(*this);
			}

			_memoryAllocator = rhs._memoryAllocator;
			_id = rhs._id;
			_blockOffset = rhs._blockOffset;
#if defined FS_DEBUG
			_rawPointerForDebug = rhs._rawPointerForDebug;
#endif
		}
		return *this;
	}

	template<typename T>
	inline MemoryAccessor2<T>& MemoryAccessor2<T>::operator=(MemoryAccessor2&& rhs)
	{
		if (this != &rhs)
		{
			if (_memoryAllocator != nullptr)
			{
				_memoryAllocator->decreaseReferenceXXX(*this);
			}

			_memoryAllocator = rhs._memoryAllocator;
			_id = rhs._id;
			_blockOffset = rhs._blockOffset;
#if defined FS_DEBUG
			_rawPointerForDebug = rhs._rawPointerForDebug;
#endif

			rhs._memoryAllocator = nullptr;
			rhs._id = kMemoryBlockIdInvalid;
			rhs._blockOffset = 0;
#if defined FS_DEBUG
			rhs._rawPointerForDebug = nullptr;
#endif
		}
		return *this;
	}

	template<typename T>
	inline const bool MemoryAccessor2<T>::isValid() const noexcept
	{
		return _memoryAllocator->isValid(*this);
	}

	template<typename T>
	inline void MemoryAccessor2<T>::setMemory(const T* const data, const uint32 count)
	{
		if (data == nullptr)
		{
			return;
		}

		if (isValid() == false)
		{
			return;
		}

		auto rawPointer = _memoryAllocator->getRawPointerXXX(*this);
		if (rawPointer != nullptr)
		{
			const uint32 arraySize = _memoryAllocator->getArraySize(*this);
			if (arraySize == 0)
			{
				memcpy(rawPointer, data, MemoryAllocator2<T>::convertBlockUnitToByteUnit(1));
			}
			else
			{
				const uint32 clampedCount = min(count, arraySize);
				memcpy(rawPointer, data, MemoryAllocator2<T>::convertBlockUnitToByteUnit(clampedCount));
			}
		}
	}

	template<typename T>
	inline void MemoryAccessor2<T>::setMemory(const T* const data, const uint32 offset, const uint32 count)
	{
		if (data == nullptr)
		{
			return;
		}

		if (isValid() == false)
		{
			return;
		}

		auto rawPointer = _memoryAllocator->getRawPointerXXX(*this);
		if (rawPointer != nullptr)
		{
			const uint32 arraySize = _memoryAllocator->getArraySize(*this);
			if (arraySize == 0)
			{
				memcpy(rawPointer, data, MemoryAllocator2<T>::convertBlockUnitToByteUnit(1));
			}
			else
			{
				const uint32 clampedOffset = min(offset, arraySize - 1);
				const uint32 clampedCount = min(count, arraySize - clampedOffset);
				memcpy(rawPointer + clampedOffset, data, MemoryAllocator2<T>::convertBlockUnitToByteUnit(clampedCount));
			}
		}
	}

	template<typename T>
	inline const T* const MemoryAccessor2<T>::getMemory() const noexcept
	{
		return 
#if defined FS_DEBUG
			_rawPointerForDebug =
#endif
			_memoryAllocator->getRawPointerXXX(*this);
	}

	template<typename T>
	inline T* const MemoryAccessor2<T>::getMemoryXXX() const noexcept
	{
		return
#if defined FS_DEBUG
			_rawPointerForDebug =
#endif
			_memoryAllocator->getRawPointerXXX(*this);
	}

	template<typename T>
	inline const uint32 MemoryAccessor2<T>::getArraySize() const noexcept
	{
		return _memoryAllocator->getArraySize(*this);
	}

	template<typename T>
	inline const uint32 MemoryAccessor2<T>::getByteSize() const noexcept
	{
		const uint32 arraySize = _memoryAllocator->getArraySize(*this);
		return (arraySize == 0) ? sizeof(T) : ((arraySize == kUint32Max) ? 0 : MemoryAllocator2<T>::convertBlockUnitToByteUnit(arraySize));
	}

#pragma endregion


#pragma region MemoryAllocator2
	template<typename T>
	inline MemoryAllocator2<T>::MemoryAllocator2()
		: _destructor{ [](const byte* const ptr) {return reinterpret_cast<const T*>(ptr)->~T(); } }
		, _rawMemory{ nullptr }
		, _memoryBlockCapacity{ 0 }
		, _memoryBlockCount{ 0 }
		, _nextMemoryBlockId{ 0 }
	{
		FS_ASSERT("김장원", (kTypeSize % kTypeAlignment) == 0, "Type Size 는 반드시 Type Alignment 의 배수여야 합니다.");
		reserve(kDefaultBlockCapacity);
	}

	template<typename T>
	inline MemoryAllocator2<T>::~MemoryAllocator2()
	{
		for (uint32 blockOffset = 0; blockOffset < _memoryBlockCapacity; ++blockOffset)
		{
			if (_isMemoryBlockInUse.get(blockOffset) == true)
			{
				deallocateInternal(blockOffset, true);
			}
		}

		FS_DELETE_ARRAY(_rawMemory);
		FS_DELETE_ARRAY(_memoryBlockArray);
	}

	template<typename T>
	template<typename ...Args>
	inline const MemoryAccessor2<T> MemoryAllocator2<T>::allocate(Args&&... args)
	{
		uint32 blockOffset = getNextAvailableBlockOffset();
		if (blockOffset == kMemoryBlockIdInvalid)
		{
			reserve(_memoryBlockCapacity * 2);

			blockOffset = getNextAvailableBlockOffset();
		}

		// Constructor
		const uint32 blockByteOffset = convertBlockUnitToByteUnit(blockOffset);
		new(&_rawMemory[blockByteOffset])T(std::forward<Args>(args)...);

		_memoryBlockArray[blockOffset]._id = _nextMemoryBlockId;
		_memoryBlockArray[blockOffset]._referenceCount = 1;
		_memoryBlockArray[blockOffset]._arraySize = 0;

		_isMemoryBlockInUse.set(blockOffset, true);

		++_memoryBlockCount;
		++_nextMemoryBlockId;
		if (kMemoryBlockIdReserved <= _nextMemoryBlockId)
		{
			FS_LOG("김장원", "이제부터는 Id 중복이 발생할 수 있습니다...!!!");
			_nextMemoryBlockId = 0;
		}

#if defined FS_DEBUG
		return MemoryAccessor2(this, _memoryBlockArray[blockOffset]._id, blockOffset, reinterpret_cast<T*>(&_rawMemory[blockByteOffset]));
#else
		return MemoryAccessor2(this, _memoryBlockArray[blockOffset]._id, blockOffset);
#endif
	}

	template<typename T>
	template<typename ...Args>
	inline const MemoryAccessor2<T> MemoryAllocator2<T>::allocateArray(const uint32 arraySize, Args && ...args)
	{
		if (arraySize == 0)
		{
			FS_ASSERT("김장원", false, "ArraySize 는 최소 1이어야 합니다!");
			return allocate(std::forward<Args>(args)...);
		}

		// TODO: arraySize 가 너무 커서 reserve 실패 시 처리
		uint32 firstBlockOffset = getNextAvailableBlockOffsetForArray(arraySize);
		while (firstBlockOffset == kMemoryBlockIdInvalid)
		{
			reserve(_memoryBlockCapacity * 2);

			firstBlockOffset = getNextAvailableBlockOffsetForArray(arraySize);
		}
		const uint32 firstBlockByteOffset = convertBlockUnitToByteUnit(firstBlockOffset);

		for (uint32 iter = 0; iter < arraySize; ++iter)
		{
			const uint32 currentBlockOffset = static_cast<uint64>(firstBlockOffset) + iter;
			const uint32 currentBlockByteOffset = convertBlockUnitToByteUnit(currentBlockOffset);

			// Constructor
			new(&_rawMemory[currentBlockByteOffset])T(std::forward<Args>(args)...);

			_memoryBlockArray[currentBlockOffset]._id = (0 == iter) ? _nextMemoryBlockId : kMemoryBlockIdArrayBody;
			_memoryBlockArray[currentBlockOffset]._referenceCount = (0 == iter) ? 1 : 0;
			_memoryBlockArray[currentBlockOffset]._arraySize = (0 == iter) ? arraySize : 0;

			_isMemoryBlockInUse.set(currentBlockOffset, true);
		}

		++_memoryBlockCount;
		++_nextMemoryBlockId;
		if (kMemoryBlockIdReserved <= _nextMemoryBlockId)
		{
			FS_LOG("김장원", "이제부터는 Id 중복이 발생할 수 있습니다...!!!");
			_nextMemoryBlockId = 0;
		}

#if defined FS_DEBUG
		return MemoryAccessor2(this, _memoryBlockArray[firstBlockOffset]._id, firstBlockOffset, reinterpret_cast<T*>(&_rawMemory[firstBlockByteOffset]));
#else
		return MemoryAccessor2(this, _memoryBlockArray[firstBlockOffset]._id, firstBlockOffset);
#endif
	}

	template<typename T>
	template<typename ...Args>
	inline const MemoryAccessor2<T> MemoryAllocator2<T>::reallocateArray(const MemoryAccessor2<T> memoryAccessor, const uint32 newArraySize, const bool keepData)
	{
		if (newArraySize == 0)
		{
			return memoryAccessor;
		}

		if (isValidInternalXXX(memoryAccessor) == false)
		{
			MemoryAccessor2<T> allocated = allocateArray(newArraySize);
			return allocated;
		}

		const uint32 oldArraySize = fs::max(memoryAccessor.getArraySize(), static_cast<uint32>(1));
		if (oldArraySize == newArraySize)
		{
			return memoryAccessor;
		}

		// TODO: arraySize 가 너무 커서 reserve 실패 시 처리
		uint32 newFirstBlockOffset = getNextAvailableBlockOffsetForArray(newArraySize);
		while (newFirstBlockOffset == kMemoryBlockIdInvalid)
		{
			reserve(_memoryBlockCapacity * 2);

			newFirstBlockOffset = getNextAvailableBlockOffsetForArray(newArraySize);
		}
		const uint32 newFirstBlockByteOffset = convertBlockUnitToByteUnit(newFirstBlockOffset);

		const uint32 oldFirstBlockOffset = memoryAccessor._blockOffset;
		const uint32 oldFirstBlockByteOffset = convertBlockUnitToByteUnit(oldFirstBlockOffset);
		if (keepData == true)
		{
			const uint32 oldArrayByteSize = convertBlockUnitToByteUnit(oldArraySize);
			const uint32 newArrayByteSize = convertBlockUnitToByteUnit(newArraySize);
			const uint32 moveByteSize = min(oldArrayByteSize, newArrayByteSize);
			memmove_s(&_rawMemory[newFirstBlockByteOffset], moveByteSize, &_rawMemory[oldFirstBlockByteOffset], moveByteSize);
		}

		if (oldArraySize < newArraySize)
		{
			for (uint32 iter = 0; iter < newArraySize; ++iter)
			{
				if (iter < oldArraySize)
				{
					const uint32 currentOldBlockOffset = static_cast<uint64>(oldFirstBlockOffset) + iter;
					_memoryBlockArray[currentOldBlockOffset]._id = kMemoryBlockIdInvalid;
					_memoryBlockArray[currentOldBlockOffset]._referenceCount = 0;
					_memoryBlockArray[currentOldBlockOffset]._arraySize = 0;

					_isMemoryBlockInUse.set(currentOldBlockOffset, false);
				}

				{
					const uint32 currentNewBlockOffset = static_cast<uint64>(newFirstBlockOffset) + iter;
					_memoryBlockArray[currentNewBlockOffset]._id = (0 == iter) ? _nextMemoryBlockId : kMemoryBlockIdArrayBody;
					_memoryBlockArray[currentNewBlockOffset]._referenceCount = (0 == iter) ? 1 : 0;
					_memoryBlockArray[currentNewBlockOffset]._arraySize = (0 == iter) ? newArraySize : 0;

					_isMemoryBlockInUse.set(currentNewBlockOffset, true);
				}
			}
		}
		else
		{
			for (uint32 iter = 0; iter < oldArraySize; ++iter)
			{
				{
					const uint32 currentOldBlockOffset = static_cast<uint64>(oldFirstBlockOffset) + iter;
					_memoryBlockArray[currentOldBlockOffset]._id = kMemoryBlockIdInvalid;
					_memoryBlockArray[currentOldBlockOffset]._referenceCount = 0;
					_memoryBlockArray[currentOldBlockOffset]._arraySize = 0;

					_isMemoryBlockInUse.set(currentOldBlockOffset, false);
				}

				if (iter < newArraySize)
				{
					const uint32 currentNewBlockOffset = static_cast<uint64>(newFirstBlockOffset) + iter;
					_memoryBlockArray[currentNewBlockOffset]._id = (0 == iter) ? _nextMemoryBlockId : kMemoryBlockIdArrayBody;
					_memoryBlockArray[currentNewBlockOffset]._referenceCount = (0 == iter) ? 1 : 0;
					_memoryBlockArray[currentNewBlockOffset]._arraySize = (0 == iter) ? newArraySize : 0;

					_isMemoryBlockInUse.set(currentNewBlockOffset, true);
				}
			}
		}

		MemoryAccessor2<T> reallocated{ this };
		reallocated._id = _nextMemoryBlockId;
		reallocated._blockOffset = newFirstBlockOffset;
#if defined FS_DEBUG
		reallocated._rawPointerForDebug = reinterpret_cast<T*>(&_rawMemory[newFirstBlockByteOffset]);
#endif

		++_nextMemoryBlockId;
		if (kMemoryBlockIdReserved <= _nextMemoryBlockId)
		{
			FS_LOG("김장원", "이제부터는 Id 중복이 발생할 수 있습니다...!!!");
			_nextMemoryBlockId = 0;
		}

		return reallocated;
	}

	template<typename T>
	inline void MemoryAllocator2<T>::deallocate(const MemoryAccessor2<T> memoryAccessor)
	{
		if (isValidInternalXXX(memoryAccessor))
		{
			deallocateInternal(memoryAccessor._blockOffset);
		}
	}

	template<typename T>
	inline void MemoryAllocator2<T>::deallocateInternal(const uint32 blockOffset, const bool forceDeallocation)
	{
		MemoryBlock& memoryBlock = _memoryBlockArray[blockOffset];
		--memoryBlock._referenceCount;

		if (forceDeallocation == true || memoryBlock._referenceCount == 0)
		{
			// Destructor
			for (uint32 iter = 0; iter < fs::max(memoryBlock._arraySize, static_cast<uint32>(1)); ++iter)
			{
				const uint32 currentBlockOffset = static_cast<uint64>(blockOffset) + iter;
				const uint32 currentBlockByteOffset = convertBlockUnitToByteUnit(currentBlockOffset);
				MemoryBlock& currentBlock = _memoryBlockArray[currentBlockOffset];
				_destructor(&_rawMemory[currentBlockByteOffset]);
				_isMemoryBlockInUse.set(currentBlockOffset, false);
				currentBlock._id = kMemoryBlockIdInvalid;
			}

			--_memoryBlockCount;
		}
	}

	template<typename T>
	inline void MemoryAllocator2<T>::increaseReferenceXXX(const MemoryAccessor2<T>& memoryAccessor)
	{
		if (isValidInternalXXX(memoryAccessor) == true)
		{
			++_memoryBlockArray[memoryAccessor._blockOffset]._referenceCount;
		}
	}

	template<typename T>
	inline void MemoryAllocator2<T>::decreaseReferenceXXX(const MemoryAccessor2<T>& memoryAccessor)
	{
		if (isValidInternalXXX(memoryAccessor) == true)
		{
			--_memoryBlockArray[memoryAccessor._blockOffset]._referenceCount;
		}
	}

	template<typename T>
	inline const bool MemoryAllocator2<T>::isValid(const MemoryAccessor2<T> memoryAccessor) const noexcept
	{
		return isValidInternalXXX(memoryAccessor);
	}

	template<typename T>
	inline const bool MemoryAllocator2<T>::isValidInternalXXX(const MemoryAccessor2<T>& memoryAccessor) const noexcept
	{
		if (_memoryBlockCapacity <= memoryAccessor._blockOffset)
		{
			return false;
		}

		if (_isMemoryBlockInUse.get(memoryAccessor._blockOffset) == true)
		{
			const MemoryBlock& memoryBlock = _memoryBlockArray[memoryAccessor._blockOffset];
			if (memoryBlock._id == memoryAccessor._id)
			{
				return true;
			}
		}
		return false;
	}

	template<typename T>
	FS_INLINE const bool MemoryAllocator2<T>::isResident(const T* const rawPointer) const noexcept
	{
		const byte* const rawPointerByte = reinterpret_cast<const byte*>(rawPointer);
		const uint32 byteCapacity = convertBlockUnitToByteUnit(_memoryBlockCapacity);
		return (_rawMemory <= rawPointerByte && (rawPointerByte - _rawMemory) < byteCapacity);
	}

	template<typename T>
	inline const uint32 MemoryAllocator2<T>::getArraySize(const MemoryAccessor2<T> memoryAccessor) const noexcept
	{
		if (false == isValidInternalXXX(memoryAccessor))
		{
			return 0;
		}

		const MemoryBlock& memoryBlock = _memoryBlockArray[memoryAccessor._blockOffset];
		return memoryBlock._arraySize;
	}

	template<typename T>
	inline T* const MemoryAllocator2<T>::getRawPointerXXX(const MemoryAccessor2<T> memoryAccessor) const noexcept
	{
		const uint32 blockByteOffset = convertBlockUnitToByteUnit(memoryAccessor._blockOffset);
		return (_memoryBlockCapacity <= memoryAccessor._blockOffset) ? nullptr : reinterpret_cast<T*>(&_rawMemory[blockByteOffset]);
	}

	template<typename T>
	inline void MemoryAllocator2<T>::reserve(const uint32 blockCapacity)
	{
		if (_memoryBlockCapacity < blockCapacity)
		{
			const uint32 oldByteCapacity = convertBlockUnitToByteUnit(_memoryBlockCapacity);
			const uint32 newByteCapacity = convertBlockUnitToByteUnit(blockCapacity);


			if (_rawMemory != nullptr)
			{
				byte* temp = FS_NEW_ARRAY(byte, oldByteCapacity);
				memcpy(temp, _rawMemory, oldByteCapacity);

				FS_DELETE_ARRAY(_rawMemory);
				_rawMemory = FS_NEW_ARRAY(byte, newByteCapacity);
				memcpy(_rawMemory, temp, oldByteCapacity);

				FS_DELETE_ARRAY(temp);
			}
			else
			{
				_rawMemory = FS_NEW_ARRAY(byte, newByteCapacity);
			}


			if (_memoryBlockArray != nullptr)
			{
				MemoryBlock* temp = FS_NEW_ARRAY(MemoryBlock, _memoryBlockCapacity);
				memcpy(temp, _memoryBlockArray, _memoryBlockCapacity);

				FS_DELETE_ARRAY(_memoryBlockArray);
				_memoryBlockArray = FS_NEW_ARRAY(MemoryBlock, blockCapacity);
				memcpy(_memoryBlockArray, temp, _memoryBlockCapacity);

				FS_DELETE_ARRAY(temp);
			}
			else
			{
				_memoryBlockArray = FS_NEW_ARRAY(MemoryBlock, blockCapacity);
			}


			_memoryBlockCapacity = blockCapacity;
			_isMemoryBlockInUse.resizeBitCount(_memoryBlockCapacity);
		}
	}

	template<typename T>
	inline const uint32 MemoryAllocator2<T>::getNextAvailableBlockOffset() const noexcept
	{
		for (uint32 blockOffset = 0; blockOffset < _memoryBlockCapacity; ++blockOffset)
		{
			if (_isMemoryBlockInUse.get(blockOffset) == false)
			{
				return blockOffset;
			}
		}
		return kMemoryBlockIdInvalid;
	}

	template<typename T>
	inline const uint32 MemoryAllocator2<T>::getNextAvailableBlockOffsetForArray(const uint32 arraySize) const noexcept
	{
		uint32 successiveAvailableBlockFirstOffset = kMemoryBlockIdInvalid;
		uint32 successiveAvailableBlockCount = 0;
		for (uint32 blockOffset = 0; blockOffset < _memoryBlockCapacity; ++blockOffset)
		{
			if (_isMemoryBlockInUse.get(blockOffset) == false)
			{
				if (successiveAvailableBlockCount == 0)
				{
					successiveAvailableBlockFirstOffset = blockOffset;
				}

				++successiveAvailableBlockCount;
				if (successiveAvailableBlockCount == arraySize)
				{
					break;
				}
			}
			else
			{
				successiveAvailableBlockCount = 0;
				successiveAvailableBlockFirstOffset = kMemoryBlockIdInvalid;
			}
		}

		if (successiveAvailableBlockCount < arraySize)
		{
			successiveAvailableBlockFirstOffset = kMemoryBlockIdInvalid;
		}
		return successiveAvailableBlockFirstOffset;
	}

	template<typename T>
	inline const uint32 MemoryAllocator2<T>::convertBlockUnitToByteUnit(const uint32 blockUnit) noexcept
	{
		return blockUnit * kTypeSize;
	}
#pragma endregion
}
