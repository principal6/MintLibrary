#pragma once


#include <Container/StackHolder.h>


namespace fs
{
	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline StackHolder<UnitByteSize, MaxUnitCount>::StackHolder()
		: _allocCountDataArray{}
		, _allocMetaDataArray{}
		, _rawByteArray{}
	{
		static_assert(0 < UnitByteSize, "UnitByteSize 를 1 이상의 값으로 지정하세요.");
		static_assert(0 < MaxUnitCount, "MaxUnitCount 를 1 이상의 값으로 지정하세요.");

		if (MaxUnitCount % kBitMaskSize != 0)
		{
			const uint32 deleteBitCount = kBitMaskSize - (MaxUnitCount % kBitMaskSize);
			const BitMaskType deleteBitMask = (static_cast<BitMaskType>(pow(2, deleteBitCount)) - 1);
			_allocMetaDataArray[kAllocMetaDataCount - 1] |= deleteBitMask;
		}
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline StackHolder<UnitByteSize, MaxUnitCount>::~StackHolder()
	{
		__noop;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline byte* StackHolder<UnitByteSize, MaxUnitCount>::registerSpace(const CountMetaDataType unitCount)
	{
		FS_ASSERT("김장원", 0 < unitCount, "!!! 0 개의 Unit 을 할당할 수는 없습니다 !!!");
		FS_ASSERT("김장원", unitCount <= kBitMaskSize, "!!! 한번에 할당 가능한 최대 Unit 수를 넘었습니다 !!!");

		uint32 allocMetaDataIndex = 0;
		uint8 bitOffset = 0;
		BitMaskType bitMask = 0;
		if (canRegister(unitCount, allocMetaDataIndex, bitOffset, bitMask) == true)
		{
			// 할당 가능 !!!
			const uint32 unitIndex = (allocMetaDataIndex * kBitMaskSize + bitOffset);
			const uint32 byteOffset = unitIndex * UnitByteSize;
			_allocCountDataArray[unitIndex] = unitCount;
			_allocMetaDataArray[allocMetaDataIndex] |= bitMask;
			return &_rawByteArray[byteOffset];
		}
		FS_ASSERT("김장원", false, "!!! StackHolder 가 가득 찼습니다 !!! 할당 실패 !!!");
		return nullptr;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline void StackHolder<UnitByteSize, MaxUnitCount>::deregisterSpace(const byte* ptr)
	{
		if (ptr == nullptr)
		{
			FS_ASSERT("김장원", false, "!!! nullptr 을 지울 수 없습니다 !!!");
			return;
		}

		const uint32 rawByteOffset = ptr - &_rawByteArray[0];
		const CountMetaDataType allocSizeDataIndex = rawByteOffset / UnitByteSize;
		CountMetaDataType& unitCount = _allocCountDataArray[allocSizeDataIndex];
		if (unitCount == 0)
		{
			FS_ASSERT("김장원", false, "!!! 할당되지 않은 ptr 을 지울 수 없습니다 !!!");
			return;
		}

		if (canDeregister(ptr, unitCount) == false)
		{
			FS_ASSERT("김장원", false, "!!! 이 StackHolder 에 할당된 ptr 이 아닙니다 !!!");
			return;
		}

		const uint32 allocMetaDataIndex = allocSizeDataIndex / kBitMaskSize;
		const uint8 bitOffset = allocSizeDataIndex % kBitMaskSize;
		const BitMaskType bitMask = (static_cast<BitMaskType>(pow(2, unitCount)) - 1);
		const BitMaskType bitMaskAligned = ((bitMask << (kBitMaskSize - unitCount)) >> bitOffset);

		_allocMetaDataArray[allocMetaDataIndex] ^= bitMaskAligned;
		unitCount = 0;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline bool StackHolder<UnitByteSize, MaxUnitCount>::canRegister(const CountMetaDataType unitCount, uint32& outAllocMetaDataIndex, uint8& outBitOffset, BitMaskType& outBitMask) const noexcept
	{
		const BitMaskType bitMaskRightAligned = (static_cast<BitMaskType>(pow(2, unitCount)) - 1);
		const BitMaskType bitMaskLeftAligned = bitMaskRightAligned << (kBitMaskSize - unitCount);
		for (uint32 allocMetaDataIndex = 0; allocMetaDataIndex < kAllocMetaDataCount; ++allocMetaDataIndex)
		{
			for (uint8 bitOffset = 0; bitOffset <= (kBitMaskSize - unitCount); ++bitOffset)
			{
				const BitMaskType offsetBitMask = bitMaskLeftAligned >> bitOffset;
				const BitMaskType maskingProcess0 = (_allocMetaDataArray[allocMetaDataIndex] ^ offsetBitMask) << bitOffset;
				const BitMaskType maskingProcess1 = maskingProcess0 >> (kBitMaskSize - unitCount);
				const BitMaskType maskingResult = (maskingProcess1 << (kBitMaskSize - unitCount)) >> bitOffset;
				if (maskingResult == offsetBitMask)
				{
					outAllocMetaDataIndex = allocMetaDataIndex;
					outBitOffset = bitOffset;
					outBitMask = offsetBitMask;
					return true;
				}
			}
		}
		return false;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline bool StackHolder<UnitByteSize, MaxUnitCount>::canDeregister(const byte* const ptr, const CountMetaDataType unitCount) const noexcept
	{
		if (ptr < &_rawByteArray[0])
		{
			return false;
		}

		const uint32 rawByteOffset = ptr - &_rawByteArray[0];
		if (kRawByteCount <= rawByteOffset)
		{
			return false;
		}

		const uint32 unitByteCount = unitCount * UnitByteSize;
		if (kRawByteCount < rawByteOffset + unitByteCount)
		{
			return false;
		}

		return true;
	}
}
