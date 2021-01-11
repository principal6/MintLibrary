#pragma once


#include "stdafx.h"
#include <FsLibrary/Memory/Allocator.h>

#include <FsLibrary/Container/BitVector.hpp>


namespace fs
{
	namespace Memory
	{
#pragma region memoryAccessor
		template<typename T>
		inline Accessor<T>::Accessor(Allocator<T>* const memoryAllocator)
			: _memoryAllocator{ memoryAllocator }
			, _id{ kMemoryBlockIdInvalid }
			, _blockOffset{ 0 }
		{
			__noop;
		}

		template<typename T>
		inline Accessor<T>::Accessor(Allocator<T>* const memoryAllocator, const MemoryBlockId id, const uint32 blockOffset)
			: _memoryAllocator{ memoryAllocator }
			, _id{ id }
			, _blockOffset{ blockOffset }
		{
			__noop;
		}

		template<typename T>
		inline Accessor<T>::Accessor(const Accessor& rhs)
			: _memoryAllocator{ rhs._memoryAllocator }
			, _id{ rhs._id }
			, _blockOffset{ rhs._blockOffset }
		{
			if (_memoryAllocator->isValidXXX(*this) == true)
			{
				_memoryAllocator->increaseReferenceXXX(*this); // 복사가 일어났으므로 ReferenceCount 가 증가해야 한다!!!
			}
		}

		template<typename T>
		inline Accessor<T>::Accessor(Accessor&& rhs) noexcept
			: _memoryAllocator{ rhs._memoryAllocator }
			, _id{ rhs._id }
			, _blockOffset{ rhs._blockOffset }
		{
			rhs.invalidateXXX();
		}

		template<typename T>
		inline Accessor<T>::~Accessor()
		{
			if (_memoryAllocator != nullptr)
			{
				_memoryAllocator->decreaseReferenceXXX(*this);
			}
		}

		template<typename T>
		inline Accessor<T>& Accessor<T>::operator=(const Accessor& rhs)
		{
			if (this != &rhs)
			{
				if (_memoryAllocator != nullptr)
				{
					_memoryAllocator->decreaseReferenceXXX(*this); // 사라질 기존 데이터의 ReferenceCount 를 낮춘다!!!
				}

				_memoryAllocator = rhs._memoryAllocator;
				_id = rhs._id;
				_blockOffset = rhs._blockOffset;

				if (_memoryAllocator != nullptr)
				{
					_memoryAllocator->increaseReferenceXXX(*this); // 복사가 일어났으므로 ReferenceCount 가 증가해야 한다!!!
				}
			}
			return *this;
		}

		template<typename T>
		inline Accessor<T>& Accessor<T>::operator=(Accessor&& rhs) noexcept
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

				rhs.invalidateXXX();
			}
			return *this;
		}

		template<typename T>
		inline const bool Accessor<T>::isValid() const noexcept
		{
			return (nullptr == _memoryAllocator) ? false : _memoryAllocator->isValid(*this);
		}

		template<typename T>
		inline void Accessor<T>::invalidateXXX()
		{
			_memoryAllocator = nullptr;
			_id = kMemoryBlockIdInvalid;
			_blockOffset = 0;
		}

		template<typename T>
		inline void Accessor<T>::setMemory(const T* const data, const uint32 count)
		{
			if (data == nullptr)
			{
				return;
			}

			if (isValid() == false)
			{
				return;
			}

			T* const rawPointer = getMemoryXXX();
			if (rawPointer != nullptr)
			{
				const uint32 arraySize = getArraySize();
				if (arraySize == 0)
				{
					rawPointer[0] = data[0];
				}
				else
				{
					const uint32 clampedCount = min(count, arraySize);

					for (uint32 iter = 0; iter < clampedCount; ++iter)
					{
						rawPointer[iter] = data[iter];
					}
				}
			}
		}

		template<typename T>
		inline void Accessor<T>::setMemory(const T* const data, const uint32 offset, const uint32 count)
		{
			if (data == nullptr)
			{
				return;
			}

			if (isValid() == false)
			{
				return;
			}

			T* const rawPointer = getMemoryXXX();
			if (rawPointer != nullptr)
			{
				const uint32 arraySize = getArraySize();
				const uint32 clampedOffset = min(offset, arraySize - 1);
				if (arraySize == 0)
				{
					rawPointer[clampedOffset] = data[0];
				}
				else
				{
					const uint32 clampedCount = min(count, arraySize - clampedOffset);

					for (uint32 iter = 0; iter < clampedCount; ++iter)
					{
						rawPointer[clampedOffset + iter] = data[iter];
					}
				}
			}
		}

		template<typename T>
		inline const T* const Accessor<T>::getMemory() const noexcept
		{
			return _memoryAllocator->getRawPointerXXX(*this);
		}

		template<typename T>
		inline T* const Accessor<T>::getMemoryXXX() const noexcept
		{
			return _memoryAllocator->getRawPointerXXX(*this);
		}

		template<typename T>
		inline const uint32 Accessor<T>::getArraySize() const noexcept
		{
			return _memoryAllocator->getArraySize(*this);
		}

#pragma endregion


#pragma region MemoryAllocator2
		template<typename T>
		inline Allocator<T>::Allocator()
			: _destructor{ [](const byte* const ptr) {return reinterpret_cast<const T*>(ptr)->~T(); } }
			, _rawMemory{ nullptr }
			, _memoryBlockCapacity{ 0 }
			, _memoryBlockArray{ nullptr }
			, _memoryBlockCount{ 0 }
			, _nextMemoryBlockId{ 0 }
			, _leastDeletedMemoryBlockIndex{ 0 }
		{
			FS_ASSERT("김장원", (kTypeSize % kTypeAlignment) == 0, "Type Size 는 반드시 Type Alignment 의 배수여야 합니다.");
			reserve(kDefaultBlockCapacity);
		}

		template<typename T>
		FS_INLINE Allocator<T>& Allocator<T>::getInstance()
		{
			static Allocator<T> allocator;
			return allocator;
		}

		template<typename T>
		inline Allocator<T>::~Allocator()
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
		inline Accessor<T> Allocator<T>::allocate(Args&&... args)
		{
			uint32 blockOffset = getNextAvailableBlockOffset();
			if (blockOffset == kMemoryBlockIdInvalid)
			{
				reserve(_memoryBlockCapacity * 2);

				blockOffset = getNextAvailableBlockOffset();
			}

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

			// Constructor
			// T 의 멤버 중에 같은 T 에 대해 MemoryAllocator 로 allocate 하는 멤버가 있다면 재귀적으로 들어올 것이므로
			// 이게 마지막에 실행되어야 합니다!!!
			const uint32 blockByteOffset = convertBlockUnitToByteUnit(blockOffset);
			new(&_rawMemory[blockByteOffset])T(std::forward<Args>(args)...);

			return Accessor(this, _memoryBlockArray[blockOffset]._id, blockOffset);
		}

		template<typename T>
		template<typename ...Args>
		inline Accessor<T> Allocator<T>::allocateArray(const uint32 arraySize, Args && ...args)
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

			++_memoryBlockCount;

			const uint32 memoryBlockId = _nextMemoryBlockId;
			++_nextMemoryBlockId;

			if (kMemoryBlockIdReserved <= _nextMemoryBlockId)
			{
				FS_LOG("김장원", "이제부터는 Id 중복이 발생할 수 있습니다...!!!");
				_nextMemoryBlockId = 0;
			}

			for (uint32 iter = 0; iter < arraySize; ++iter)
			{
				const uint32 currentBlockOffset = static_cast<uint64>(firstBlockOffset) + iter;
				const uint32 currentBlockByteOffset = convertBlockUnitToByteUnit(currentBlockOffset);

				_memoryBlockArray[currentBlockOffset]._id = (0 == iter) ? memoryBlockId : kMemoryBlockIdArrayBody;
				_memoryBlockArray[currentBlockOffset]._referenceCount = (0 == iter) ? 1 : 0;
				_memoryBlockArray[currentBlockOffset]._arraySize = (0 == iter) ? arraySize : 0;

				_isMemoryBlockInUse.set(currentBlockOffset, true);

				// Constructor
				// T 의 멤버 중에 같은 T 에 대해 MemoryAllocator 로 allocate 하는 멤버가 있다면 재귀적으로 들어올 것이므로
				// 이게 마지막에 실행되어야 합니다!!!
				new(&_rawMemory[currentBlockByteOffset])T(std::forward<Args>(args)...);
			}

			return Accessor(this, _memoryBlockArray[firstBlockOffset]._id, firstBlockOffset);
		}

		template<typename T>
		template<typename ...Args>
		inline Accessor<T> Allocator<T>::reallocateArray(Accessor<T> memoryAccessor, const uint32 newArraySize, const bool keepData)
		{
			if (newArraySize == 0)
			{
				return memoryAccessor;
			}

			if (isValidXXX(memoryAccessor) == false)
			{
				return allocateArray(newArraySize);
			}

			const uint32 oldArraySize = fs::max(memoryAccessor.getArraySize(), static_cast<uint32>(1));
			if (oldArraySize == newArraySize)
			{
				return memoryAccessor;
			}

			uint32 newFirstBlockOffset = getNextAvailableBlockOffsetForArray(newArraySize);
			while (newFirstBlockOffset == kMemoryBlockIdInvalid)
			{
				// TODO: arraySize 가 너무 커서 reserve 실패 시 처리
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
					const uint32 currentNewBlockOffset = static_cast<uint64>(newFirstBlockOffset) + iter;
					{
						_memoryBlockArray[currentNewBlockOffset]._id = (0 == iter) ? _nextMemoryBlockId : kMemoryBlockIdArrayBody;
						_memoryBlockArray[currentNewBlockOffset]._referenceCount = (0 == iter) ? 1 : 0;
						_memoryBlockArray[currentNewBlockOffset]._arraySize = (0 == iter) ? newArraySize : 0;

						_isMemoryBlockInUse.set(currentNewBlockOffset, true);
					}

					if (iter < oldArraySize)
					{
						// Old Array 제거

						const uint32 currentOldBlockOffset = static_cast<uint64>(oldFirstBlockOffset) + iter;
						_memoryBlockArray[currentOldBlockOffset]._id = kMemoryBlockIdInvalid;
						_memoryBlockArray[currentOldBlockOffset]._referenceCount = 0;
						_memoryBlockArray[currentOldBlockOffset]._arraySize = 0;

						_isMemoryBlockInUse.set(currentOldBlockOffset, false);
					}
					else
					{
						// Old Array 에서 없었지만 New Array 에 추가된 항목들 생성자 호출!!!

						// Constructor
						const uint32 currentNewBlockByteOffset = convertBlockUnitToByteUnit(currentNewBlockOffset);
						new(&_rawMemory[currentNewBlockByteOffset])T();
					}
				}
			}
			else
			{
				// New Array 는 Old Array 보다 크기가 줄었으므로
				// 생성자 호출 불필요.

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

			memoryAccessor._id = _nextMemoryBlockId;
			memoryAccessor._blockOffset = newFirstBlockOffset;

			++_nextMemoryBlockId;
			if (kMemoryBlockIdReserved <= _nextMemoryBlockId)
			{
				FS_LOG("김장원", "이제부터는 Id 중복이 발생할 수 있습니다...!!!");
				_nextMemoryBlockId = 0;
			}

			return memoryAccessor;
		}

		template<typename T>
		inline void Allocator<T>::deallocate(Accessor<T>& memoryAccessor)
		{
			if (isValidXXX(memoryAccessor) == true)
			{
				deallocateInternal(memoryAccessor._blockOffset, true);
			}
			else
			{
				FS_ASSERT("김장원", false, "Invalid 한 MemoryAccessor 를 deallocate 하려고 시도합니다!!!");
			}

			memoryAccessor.invalidateXXX();
		}

		template<typename T>
		inline void Allocator<T>::deallocateInternal(const uint32 blockOffset, const bool forceDeallocation)
		{
			const MemoryBlock& memoryBlock = _memoryBlockArray[blockOffset];
			if (forceDeallocation == true || memoryBlock._referenceCount == 0)
			{
				if (1 < memoryBlock._referenceCount)
				{
					FS_ASSERT("김장원", false, "발생하면 안 되는 상황!!!");
				}

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

				_leastDeletedMemoryBlockIndex = fs::min(_leastDeletedMemoryBlockIndex, blockOffset);
			}
			else
			{
				FS_ASSERT("김장원", false, "ReferenceCount 가 잘못된 MemoryBlock 이 있습니다!!!");
			}
		}

		template<typename T>
		inline void Allocator<T>::increaseReferenceXXX(const Accessor<T>& memoryAccessor)
		{
			if (isValidXXX(memoryAccessor) == true)
			{
				++_memoryBlockArray[memoryAccessor._blockOffset]._referenceCount;
			}
			else
			{
				FS_ASSERT("김장원", false, "발생하면 안 되는 상황!!!");
			}
		}

		template<typename T>
		inline void Allocator<T>::decreaseReferenceXXX(const Accessor<T>& memoryAccessor)
		{
			if (isValidXXX(memoryAccessor) == true)
			{
				if (0 == _memoryBlockArray[memoryAccessor._blockOffset]._referenceCount)
				{
					FS_ASSERT("김장원", false, "발생하면 안 되는 상황!!!");
				}

				--_memoryBlockArray[memoryAccessor._blockOffset]._referenceCount;

				if (_memoryBlockArray[memoryAccessor._blockOffset]._referenceCount == 0)
				{
					deallocateInternal(memoryAccessor._blockOffset);
				}
			}
		}

		template<typename T>
		inline const bool Allocator<T>::isValid(const Accessor<T> memoryAccessor) const noexcept
		{
			return isValidXXX(memoryAccessor);
		}

		template<typename T>
		inline const bool Allocator<T>::isValidXXX(const Accessor<T>& memoryAccessor) const noexcept
		{
			return memoryAccessor._id != kMemoryBlockIdInvalid && isResidentXXX(memoryAccessor) && (0 < _memoryBlockArray[memoryAccessor._blockOffset]._referenceCount);
		}

		template<typename T>
		inline const bool Allocator<T>::isResidentXXX(const Accessor<T>& memoryAccessor) const noexcept
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
		FS_INLINE const bool Allocator<T>::isResident(const T* const rawPointer) const noexcept
		{
			const byte* const rawPointerByte = reinterpret_cast<const byte*>(rawPointer);
			const uint32 byteCapacity = convertBlockUnitToByteUnit(_memoryBlockCapacity);
			return (_rawMemory <= rawPointerByte && (rawPointerByte - _rawMemory) < byteCapacity);
		}

		template<typename T>
		inline const uint32 Allocator<T>::getArraySize(const Accessor<T> memoryAccessor) const noexcept
		{
			if (false == isValidXXX(memoryAccessor))
			{
				return 0;
			}

			const MemoryBlock& memoryBlock = _memoryBlockArray[memoryAccessor._blockOffset];
			return memoryBlock._arraySize;
		}

		template<typename T>
		inline T* const Allocator<T>::getRawPointerXXX(const Accessor<T> memoryAccessor) const noexcept
		{
			const uint32 blockByteOffset = convertBlockUnitToByteUnit(memoryAccessor._blockOffset);
			return (_memoryBlockCapacity <= memoryAccessor._blockOffset) ? nullptr : reinterpret_cast<T*>(&_rawMemory[blockByteOffset]);
		}

		template<typename T>
		inline void Allocator<T>::reserve(const uint32 blockCapacity)
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
					for (uint32 iter = 0; iter < _memoryBlockCapacity; ++iter)
					{
						temp[iter] = _memoryBlockArray[iter];
					}

					FS_DELETE_ARRAY(_memoryBlockArray);
					_memoryBlockArray = FS_NEW_ARRAY(MemoryBlock, blockCapacity);
				
					for (uint32 iter = 0; iter < _memoryBlockCapacity; ++iter)
					{
						_memoryBlockArray[iter] = temp[iter];
					}

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
		inline const uint32 Allocator<T>::getNextAvailableBlockOffset() const noexcept
		{
			for (uint32 blockOffset = _leastDeletedMemoryBlockIndex; blockOffset < _memoryBlockCapacity; ++blockOffset)
			{
				if (_isMemoryBlockInUse.get(blockOffset) == false)
				{
					return blockOffset;
				}
			}
			return kUint32Max;
		}

		template<typename T>
		inline const uint32 Allocator<T>::getNextAvailableBlockOffsetForArray(const uint32 arraySize) const noexcept
		{
			uint32 successiveAvailableBlockCount = 0;
			uint32 successiveAvailableBlockFirstOffset = kUint32Max;
			

			uint32 bitAt = _leastDeletedMemoryBlockIndex;
			while (true)
			{
				const uint32 bitAlignmentStride = kBitsPerByte - BitVector::getBitOffsetByBitAt(bitAt);
				const uint32 byteIndex = BitVector::getByteAtByBitAt(bitAt);
				if (_memoryBlockCapacity <= bitAt)
				{
					break;
				}

				const byte bits = _isMemoryBlockInUse.getByte(byteIndex);
				if (bits == 255)
				{
					successiveAvailableBlockCount = 0;
					successiveAvailableBlockFirstOffset = kUint32Max;

					bitAt += bitAlignmentStride;
					continue;
				}
				else if (bits == 0)
				{
					if (successiveAvailableBlockCount == 0)
					{
						successiveAvailableBlockFirstOffset = bitAt;
					}

					successiveAvailableBlockCount += kBitsPerByte;
					if (arraySize <= successiveAvailableBlockCount)
					{
						return successiveAvailableBlockFirstOffset;
					}

					bitAt += bitAlignmentStride;
					continue;
				}
				else
				{
					const uint32 bitOffset = BitVector::getBitOffsetByBitAt(bitAt);
					for (uint32 iter = bitOffset; iter < kBitsPerByte; ++iter)
					{
						const uint8 bitMaskOneAt = BitVector::getBitMaskOneAt(iter);
						const bool isInUse = (bits & bitMaskOneAt);
						if (isInUse == false)
						{
							if (successiveAvailableBlockCount == 0)
							{
								successiveAvailableBlockFirstOffset = bitAt + iter - bitOffset;
							}

							++successiveAvailableBlockCount;
						}
						else
						{
							successiveAvailableBlockCount = 0;
							successiveAvailableBlockFirstOffset = kUint32Max;
						}

						if (arraySize <= successiveAvailableBlockCount)
						{
							return successiveAvailableBlockFirstOffset;
						}
					}
					
					if (arraySize <= successiveAvailableBlockCount)
					{
						return successiveAvailableBlockFirstOffset;
					}
				}

				bitAt += bitAlignmentStride;
			}
			

			if (successiveAvailableBlockCount < arraySize)
			{
				successiveAvailableBlockFirstOffset = kUint32Max;
			}
			return successiveAvailableBlockFirstOffset;
		}

		template<typename T>
		inline const uint32 Allocator<T>::convertBlockUnitToByteUnit(const uint32 blockUnit) noexcept
		{
			return blockUnit * kTypeSize;
		}
#pragma endregion
	}
}
