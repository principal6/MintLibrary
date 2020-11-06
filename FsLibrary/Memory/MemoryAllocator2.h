#pragma once


#ifndef FS_MEMORY_ALLOCATOR_2_H
#define FS_MEMORY_ALLOCATOR_2_H


#include <CommonDefinitions.h>
#include <Container\BitVector.h>


namespace fs
{
	template <typename T>
	class MemoryAccessor2;

	template <typename T>
	class MemoryAllocator2;


	using MemoryBlockId				= uint32;


	static constexpr MemoryBlockId	kMemoryBlockIdInvalid	= kUint32Max;
	static constexpr MemoryBlockId	kMemoryBlockIdArrayBody	= kMemoryBlockIdInvalid - 1;
	static constexpr MemoryBlockId	kMemoryBlockIdReserved	= kMemoryBlockIdInvalid - 8;


	template <typename T>
	class MemoryAccessor2 final
	{
		friend MemoryAllocator2;

#if defined FS_DEBUG
	public:
										MemoryAccessor2(MemoryAllocator2<T>* const memoryAllocator)
											: _memoryAllocator{ memoryAllocator }
											, _id{ kMemoryBlockIdInvalid }
											, _blockOffset{ 0 }
											, _rawPointerForDebug{ nullptr }
										{
											__noop;
										}

	private:
										MemoryAccessor2(MemoryAllocator2<T>* const memoryAllocator, const MemoryBlockId id, const uint32 blockOffset, T* const rawPointerForDebug)
											: _memoryAllocator{ memoryAllocator }
											, _id{ id }
											, _blockOffset{ blockOffset }
											, _rawPointerForDebug{ rawPointerForDebug }
#else
	public:
										MemoryAccessor2(MemoryAllocator2<T>* const memoryAllocator)
											: _memoryAllocator{ memoryAllocator }
											, _id{ kMemoryBlockIdInvalid }
											, _blockOffset{ 0 }
										{
											__noop;
										}

	private:
										MemoryAccessor2(MemoryAllocator2<T>* const memoryAllocator, const MemoryBlockId id, const uint32 blockOffset)
											: _memoryAllocator{ memoryAllocator }
											, _id{ id }
											, _blockOffset{ blockOffset }
#endif
										{
											__noop;
										}

	public:
										MemoryAccessor2(const MemoryAccessor2& rhs);
										MemoryAccessor2(MemoryAccessor2&& rhs) noexcept;

	public:
										~MemoryAccessor2();

	public:
		MemoryAccessor2&				operator=(const MemoryAccessor2& rhs);
		MemoryAccessor2&				operator=(MemoryAccessor2&& rhs) noexcept;

	public:
		const bool						isValid() const noexcept;

	private:
		void							invalidateXXX();

	public:
		void							setMemory(const T* const data, const uint32 count = 1);
		void							setMemory(const T* const data, const uint32 offset, const uint32 count);
		const T* const					getMemory() const noexcept;
		T* const						getMemoryXXX() const noexcept;
		const uint32					getArraySize() const noexcept;
		const uint32					getByteSize() const noexcept;

	private:
		MemoryAllocator2<T>*			_memoryAllocator;
		MemoryBlockId					_id;
		uint32							_blockOffset;
#if defined FS_DEBUG
		mutable T*						_rawPointerForDebug;
#endif
	};


	template <typename T>
	class MemoryAllocator2 final
	{
		friend MemoryAccessor2;

		typedef void(*DestructorFunction)(const byte* const);
		static constexpr uint32					kDefaultBlockCapacity = 16;

		class MemoryBlock final
		{
		public:
			MemoryBlockId						_id{ kMemoryBlockIdInvalid };
			uint32								_referenceCount{ 0 };
			uint32								_arraySize{ 0 };
		};

	public:
												MemoryAllocator2();
												~MemoryAllocator2();

	public:
		template <typename ...Args>
		MemoryAccessor2<T>						allocate(Args&&... args);

		template <typename ...Args>
		MemoryAccessor2<T>						allocateArray(const uint32 arraySize, Args&&... args);

		template <typename ...Args>
		MemoryAccessor2<T>						reallocateArray(MemoryAccessor2<T> memoryAccessor, const uint32 newArraySize, const bool keepData);

		void									deallocate(MemoryAccessor2<T>& memoryAccessor);

	private:
		void									deallocateInternal(const uint32 blockOffset, const bool forceDeallocation = false);

	private:
		void									increaseReferenceXXX(const MemoryAccessor2<T>& memoryAccessor);
		void									decreaseReferenceXXX(const MemoryAccessor2<T>& memoryAccessor);

	public:
		const bool								isValid(const MemoryAccessor2<T> memoryAccessor) const noexcept;

	private:
		const bool								isValidXXX(const MemoryAccessor2<T>& memoryAccessor) const noexcept;
		const bool								isResidentXXX(const MemoryAccessor2<T>& memoryAccessor) const noexcept;

	public:
		const bool								isResident(const T* const rawPointer) const noexcept;

	public:
		const uint32							getArraySize(const MemoryAccessor2<T> memoryAccessor) const noexcept;

	private:
		T* const								getRawPointerXXX(const MemoryAccessor2<T> memoryAccessor) const noexcept;

	public:
		void									reserve(const uint32 blockCapacity);

	private:
		const uint32							getNextAvailableBlockOffset() const noexcept;
		const uint32							getNextAvailableBlockOffsetForArray(const uint32 arraySize) const noexcept;
		static const uint32						convertBlockUnitToByteUnit(const uint32 blockUnit) noexcept;

	private:
		static constexpr uint32					kTypeAlignment	= alignof(T);
		static constexpr uint32					kTypeSize		= sizeof(T);
		DestructorFunction						_destructor;

	private:
		byte*									_rawMemory;
		uint32									_memoryBlockCapacity;

		MemoryBlock*							_memoryBlockArray;
		uint32									_memoryBlockCount;
		BitVector								_isMemoryBlockInUse;

		MemoryBlockId							_nextMemoryBlockId;
	};
}


#endif // !FS_MEMORY_ALLOCATOR_2_H
