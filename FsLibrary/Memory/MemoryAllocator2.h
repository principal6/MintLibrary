#pragma once


#ifndef FS_MEMORY_ALLOCATOR_2_H
#define FS_MEMORY_ALLOCATOR_2_H


#include <CommonDefinitions.h>
#include <Container\BitVector.h>


namespace fs
{
	template <typename T>
	class MemoryAllocator2;

	template <typename T>
	class MemoryAccessor2;


	using MemoryBlockId				= uint32;


	static constexpr MemoryBlockId	kMemoryBlockIdInvalid = kUint32Max;
	static constexpr MemoryBlockId	kMemoryBlockIdArrayBody = kMemoryBlockIdInvalid - 1;
	static constexpr MemoryBlockId	kMemoryBlockIdReserved = kMemoryBlockIdInvalid - 8;


	template <typename T>
	class MemoryAccessor2 final
	{
		friend MemoryAllocator2<T>;

	private:
#if defined FS_DEBUG
										MemoryAccessor2(MemoryAllocator2<T>* memoryAllocator, const MemoryBlockId id, const uint32 blockOffset, T* const rawPointerForDebug)
											: _memoryAllocator{ memoryAllocator }
											, _id{ id }
											, _blockOffset{ blockOffset }
											, _rawPointerForDebug{ rawPointerForDebug }
#else
										MemoryAccessor2(MemoryAllocator2<T>* memoryAllocator, const MemoryBlockId id, const uint32 blockOffset)
											: _memoryAllocator{ memoryAllocator }
											, _id{ id }
											, _blockOffset{ blockOffset }
#endif
										{
											__noop;
										}

	public:
										MemoryAccessor2(const MemoryAccessor2& rhs);
										MemoryAccessor2(MemoryAccessor2&& rhs);

	public:
										~MemoryAccessor2();

	public:
		MemoryAccessor2&				operator=(const MemoryAccessor2& rhs);
		MemoryAccessor2&				operator=(MemoryAccessor2&& rhs);

	public:
		const bool						isValid() const noexcept;

	public:
		void							setMemory(const T* const data);
		const T* const					getMemory() const noexcept;

	private:
		MemoryAllocator2<T>*			_memoryAllocator;
		MemoryBlockId					_id;
		uint32							_blockOffset;
#if defined FS_DEBUG
		T*								_rawPointerForDebug;
#endif
	};


	template <typename T>
	class MemoryAllocator2 final
	{
		friend class MemoryAccessor2<T>;

		typedef void(*DestructorFunction)(const byte* const);
		static constexpr uint32		kDefaultBlockCapacity = 16;

		class MemoryBlock final
		{
		public:
			MemoryBlockId				_id{ kMemoryBlockIdInvalid };
			uint32						_referenceCount{ 0 };
			uint32						_arraySize{ 0 };
		};

	public:
										MemoryAllocator2();
										~MemoryAllocator2();

	public:
		template <typename ...Args>
		const MemoryAccessor2<T>		allocate(Args&&... args);

		template <typename ...Args>
		const MemoryAccessor2<T>		allocateArray(const uint32 arraySize, Args&&... args);

		void							deallocate(const MemoryAccessor2<T>& memoryAccessor);

	private:
		void							deallocateInternal(const uint32 blockOffset, const bool forceDeallocation = false);

	private:
		void							increaseReferenceXXX(const MemoryAccessor2<T>& memoryAccessor);
		void							decreaseReferenceXXX(const MemoryAccessor2<T>& memoryAccessor);

	public:
		const bool						isValid(const MemoryAccessor2<T>& memoryAccessor) const noexcept;

	private:
		T* const						getRawPointerXXX(const MemoryAccessor2<T>& memoryAccessor) const noexcept;

	public:
		void							reserve(const uint32 blockCapacity);

	private:
		const uint32					getNextAvailableBlockOffset() const noexcept;
		const uint32					getNextAvailableBlockOffsetForArray(const uint32 arraySize) const noexcept;
		const uint32					convertBlockUnitToByteUnit(const uint32 blockUnit) const noexcept;

	private:
		DestructorFunction				_destructor;
		uint32							_typeAlignment;
		uint32							_typeSize;

	private:
		byte*							_rawMemory;
		uint32							_memoryBlockCapacity;

		MemoryBlock*					_memoryBlockArray;
		uint32							_memoryBlockCount;
		BitVector						_isMemoryBlockInUse;

		MemoryBlockId					_nextMemoryBlockId;
	};
}


#endif // !FS_MEMORY_ALLOCATOR_2_H
