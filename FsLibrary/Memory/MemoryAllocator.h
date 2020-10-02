#pragma once


#ifndef FS_ALLOCATOR_H
#define FS_ALLOCATOR_H


#include <CommonDefinitions.h>
#include <Math\MathCommon.h>
#include <Container\BitVector.h>


//#define FS_MEMORY_USE_IS_SHARED_FLAG
//#define FS_MEMORY_CALCULATE_TEST_VALUES


namespace fs
{
	class MemoryAccessor;
	class MemoryAllocator;


	using MemoryBucketRawIdType = uint32;
	class MemoryBucketId final
	{
		friend MemoryAllocator;

		static constexpr MemoryBucketRawIdType		kInvalidMemoryBucketRawId{ kUint32Max };

	private:
													MemoryBucketId() = default;
	
	public:
													MemoryBucketId(const MemoryBucketId& rhs) = default;
													MemoryBucketId(MemoryBucketId&& rhs) = default;
													~MemoryBucketId() = default;

	public:
		MemoryBucketId&								operator=(const MemoryBucketId& rhs) = default;
		MemoryBucketId&								operator=(MemoryBucketId&& rhs) = default;

	public:
		FS_INLINE const bool						operator==(const MemoryBucketId& rhs) const noexcept
		{
			return _rawId == rhs._rawId;			
		}

	public:
		FS_INLINE const bool						isValid() const noexcept
		{
			return _rawId != kInvalidMemoryBucketRawId;
		}

	private:
		FS_INLINE void								assignIdXXX() noexcept
		{
			++_lastRawId;
			_rawId = _lastRawId;
		}

		FS_INLINE const MemoryBucketRawIdType		getRawIdXXX() const noexcept
		{
			return _rawId;
		}

	private:
		MemoryBucketRawIdType						_rawId{ kInvalidMemoryBucketRawId };

	private:
		static std::atomic<MemoryBucketRawIdType>	_lastRawId;

	public:
		static const MemoryBucketId					kInvalidMemoryBucketId;
	};


	class MemoryAccessor
	{
		friend MemoryAllocator;

	public:
									MemoryAccessor();
									MemoryAccessor(const MemoryAccessor& rhs);
									MemoryAccessor(MemoryAccessor&& rhs) = default;
									~MemoryAccessor();

	private:
									MemoryAccessor(const MemoryBucketId bucketId, MemoryAllocator* const memoryAllocator);

	public:
		MemoryAccessor&				operator=(const MemoryAccessor& rhs);
		MemoryAccessor&				operator=(MemoryAccessor&& rhs) = default;

	public:
		const bool					isValid() const noexcept;
		void						setMemory(const byte* const content);
		const byte*					getMemory() const noexcept;
		const uint32				getByteSize() const noexcept;

	private:
		MemoryBucketId				_bucketId;
		MemoryAllocator*			_memoryAllocator;
	};
	

	class MemoryAllocator final
	{
		friend MemoryAccessor;

		static constexpr uint32		kDefaultBlockSize = 4; // The bigger the number is, the more internal fragmentations will happen.
		static constexpr uint32		kDefaultInitialBlockCapacity = 256;
		static constexpr uint32		kTheoreticalCacheLineSizeInBytes = 64;
		static constexpr uint32		kDefaultReorganizationCycleInBytes = kTheoreticalCacheLineSizeInBytes * 16;
		
		class MemoryBucket final
		{
			friend MemoryAllocator;

		private:
			MemoryBucketId	_id;
			uint32			_byteSize;
			uint32			_blockOffset;
			uint32			_blockCount;
#if defined FS_MEMORY_USE_IS_SHARED_FLAG
			struct
			{
				uint32		_referenceCount : 31;
				uint32		_isShared : 1;
			};
#else
			uint32			_referenceCount;
#endif
		};

#if defined FS_MEMORY_CALCULATE_TEST_VALUES
		static constexpr uint64		___TEST_BLOCK_COUNT = 1024 * 1024 * 256;
		
		static constexpr uint64		____TEST_BLOCK_METADATA_BYTES = ___TEST_BLOCK_COUNT / kBitsPerByte;
		static constexpr uint64		____TEST_BLOCK_METADATA_KB = ____TEST_BLOCK_METADATA_BYTES / 1024;
		static constexpr uint64		___TEST_BLOCK_METADATA_MB = ____TEST_BLOCK_METADATA_KB / 1024;

		static constexpr uint64		____TEST_ALLOCATED_MEMORY_BYTES = ___TEST_BLOCK_COUNT * kDefaultBlockSize;
		static constexpr uint64		____TEST_ALLOCATED_MEMORY_KB = ____TEST_ALLOCATED_MEMORY_BYTES / 1024;
		static constexpr uint64		___TEST_ALLOCATED_MEMORY_MB = ____TEST_ALLOCATED_MEMORY_KB / 1024;
#endif

	public:
																	MemoryAllocator();
																	MemoryAllocator(const MemoryAllocator& rhs) = delete;
																	~MemoryAllocator();

	private:
		const bool													operator=(const MemoryAllocator& rhs) = delete;

	public:
#if defined FS_MEMORY_USE_IS_SHARED_FLAG
		MemoryAccessor												allocate(const uint32 byteSize, const bool isShared = false);
#else
		MemoryAccessor												allocate(const uint32 byteSize);
#endif
		void														deallocate(MemoryAccessor& memoryAccessor);

	private:
		const bool													canAllocate(const uint32 blockCount) const noexcept;
		void														reserve(const uint32 blockCapacity);

	private:
		void														cleanUpRawMemory();

	private:
		const size_t												convertBlockUnitToByteUnit(const uint32 blockUnit) const noexcept;

	private:
		const bool													isValid(const MemoryBucketId bucketId) const noexcept;
		void														setMemoryXXX(const MemoryBucketId bucketId, const byte* const content);
		const byte*													getMemoryXXX(const MemoryBucketId bucketId) const;
		const uint32												getByteSize(const MemoryBucketId bucketId) const;
		void														increaseReferenceXXX(const MemoryBucketId bucketId);
		void														decreaseReferenceXXX(const MemoryBucketId bucketId, MemoryAccessor& memoryAccessor);

	private:
		const uint32												kBlockSize;
		std::mutex													_mutex;

	private:
		uint32														_rawCapacity;
		byte*														_rawMemory;
		
	private:
		uint32														_nextAvailableBlockIndex;
		BitVector													_blockInUseArray;

	private:
		std::unordered_map<MemoryBucketRawIdType, MemoryBucket>		_bucketMap;
	};
}


#endif // !FS_ALLOCATOR_H