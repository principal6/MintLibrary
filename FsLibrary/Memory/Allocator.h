#pragma once


#ifndef FS_MEMORY_ALLOCATOR_2_H
#define FS_MEMORY_ALLOCATOR_2_H


#include <CommonDefinitions.h>
#include <Container\BitVector.h>


namespace fs
{
	namespace Memory
	{
		template <typename T>
		class Accessor;

		template <typename T>
		class Allocator;


		using MemoryBlockId										= uint32;


		static constexpr MemoryBlockId kMemoryBlockIdInvalid	= kUint32Max;
		static constexpr MemoryBlockId kMemoryBlockIdArrayBody	= kMemoryBlockIdInvalid - 1;
		static constexpr MemoryBlockId kMemoryBlockIdReserved	= kMemoryBlockIdInvalid - 8;


		template <typename T>
		class Accessor final
		{
			friend Allocator;

		public:
											Accessor(Allocator<T>* const memoryAllocator);

		private:
											Accessor(Allocator<T>* const memoryAllocator, const MemoryBlockId id, const uint32 blockOffset);

		public:
											Accessor(const Accessor& rhs);
											Accessor(Accessor&& rhs) noexcept;

		public:
											~Accessor();

		public:
			Accessor&						operator=(const Accessor& rhs);
			Accessor&						operator=(Accessor&& rhs) noexcept;

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

		private:
			Allocator<T>*					_memoryAllocator;
			MemoryBlockId					_id;
			uint32							_blockOffset;
		};


		template <typename T>
		class Allocator final
		{
			friend Accessor;

			typedef									void (*DestructorFunction)(const byte* const);

			static constexpr uint32					kDefaultBlockCapacity = 16;

			class MemoryBlock final
			{
			public:
				MemoryBlockId						_id{ kMemoryBlockIdInvalid };
				uint32								_referenceCount{ 0 };
				uint32								_arraySize{ 0 };
			};

		public:
			static Allocator&						getInstance();

		private:
													Allocator();
													Allocator(const Allocator& rhs) = delete;
													Allocator(Allocator&& rhs) noexcept = delete;

		public:
													~Allocator();

		private:
			Allocator&								operator=(const Allocator& rhs) = delete;
			Allocator&								operator=(Allocator&& rhs) noexcept = delete;

		// TODO: these functions must be thread-safe!!!
		public:
			template <typename ...Args>
			Accessor<T>								allocate(Args&&... args);

			template <typename ...Args>
			Accessor<T>								allocateArray(const uint32 arraySize, Args&&... args);

			template <typename ...Args>
			Accessor<T>								reallocateArray(Accessor<T> memoryAccessor, const uint32 newArraySize, const bool keepData);

			void									deallocate(Accessor<T>& memoryAccessor);

		// TODO: this function must be thread-safe!!!
		private:
			void									deallocateInternal(const uint32 blockOffset, const bool forceDeallocation = false);

		// TODO: these functions must be thread-safe!!!
		private:
			void									increaseReferenceXXX(const Accessor<T>& memoryAccessor);
			void									decreaseReferenceXXX(const Accessor<T>& memoryAccessor);

		public:
			const bool								isValid(const Accessor<T> memoryAccessor) const noexcept;

		private:
			const bool								isValidXXX(const Accessor<T>& memoryAccessor) const noexcept;
			const bool								isResidentXXX(const Accessor<T>& memoryAccessor) const noexcept;

		public:
			const bool								isResident(const T* const rawPointer) const noexcept;

		public:
			const uint32							getArraySize(const Accessor<T> memoryAccessor) const noexcept;

		private:
			T* const								getRawPointerXXX(const Accessor<T> memoryAccessor) const noexcept;
			
		// TODO: this function must be thread-safe!!!
		public:
			void									reserve(const uint32 blockCapacity);

		private:
			const uint32							getNextAvailableBlockOffset() const noexcept;
			const uint32							getNextAvailableBlockOffsetForArray(const uint32 arraySize) const noexcept;
			static const uint32						convertBlockUnitToByteUnit(const uint32 blockUnit) noexcept;

		private:
			static constexpr uint32					kTypeAlignment	= alignof(T);
			static constexpr uint32					kTypeSize		= sizeof(T);

		private:
			DestructorFunction						_destructor;

		private:
			byte*									_rawMemory;

			MemoryBlock*							_memoryBlockArray;
			uint32									_memoryBlockCapacity;
			uint32									_memoryBlockCount;
			BitVector								_isMemoryBlockInUse;
			MemoryBlockId							_nextMemoryBlockId;
		};
	}
}


#endif // !FS_MEMORY_ALLOCATOR_2_H
