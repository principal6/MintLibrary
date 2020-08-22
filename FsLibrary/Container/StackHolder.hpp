﻿#pragma once


#include <Container/StackHolder.h>


namespace fs
{
	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline StackHolder<UnitByteSize, MaxUnitCount>::StackHolder()
		: _allocationSizeArray{}
		, _allocationMeta{}
		, _byteArray{}
	{
		if (MaxUnitCount % 8 != 0)
		{
			const uint32 deleteBitCount = 8 - (MaxUnitCount % 8);
			const byte deleteBitMask = (static_cast<byte>(pow(2, deleteBitCount)) - 1);
			_allocationMeta[kMetaDataSize - 1] |= deleteBitMask;
		}
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline StackHolder<UnitByteSize, MaxUnitCount>::~StackHolder()
	{
		__noop;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline byte* StackHolder<UnitByteSize, MaxUnitCount>::registerSpace(const byte unitCount)
	{
		FS_ASSERT("김장원", 0 < unitCount, "!!! 0 개의 Unit 을 할당할 수는 없습니다 !!!");
		FS_ASSERT("김장원", unitCount < 256, "!!! 한번에 할당 가능한 최대 Unit 수는 255 입니다 !!!");

		uint32 allocationMetaIndex = 0;
		byte alignmentIndex = 0;
		byte bitMask = 0;
		if (canRegister(0, unitCount, allocationMetaIndex, alignmentIndex, bitMask) == true)
		{
			// 할당 가능 !!!
			const uint32 unitIndex = (allocationMetaIndex * 8 + alignmentIndex);
			const uint32 byteAt = unitIndex * UnitByteSize;
			_allocationSizeArray[unitIndex] = unitCount;
			_allocationMeta[allocationMetaIndex] |= bitMask;
			return &_byteArray[byteAt];
		}
		FS_ASSERT("김장원", false, "!!! StackHolder 가 가득 찼습니다 !!! 할당 실패 !!!");
		return nullptr;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline void StackHolder<UnitByteSize, MaxUnitCount>::deregisterSpace(const byte* ptr)
	{
		const uint32 byteOffset = ptr - &_byteArray[0];
		const uint32 unitOffset = byteOffset / UnitByteSize;
		byte& unitCount = _allocationSizeArray[unitOffset];
		if (unitCount == 0)
		{
			FS_ASSERT("김장원", false, "!!! 할당되지 않은 ptr 를 지울 수 없습니다 !!!");
			return;
		}

		if (canDeregister(ptr, unitCount) == false)
		{
			FS_ASSERT("김장원", false, "!!! 이 StackHolder 에 할당된 ptr 이 아닙니다 !!!");
			return;
		}

		const uint32 allocationMetaIndexOffset = unitOffset / 8;
		const byte bitOffset = unitOffset % 8;
		const byte bitMask = (static_cast<byte>(pow(2, unitCount)) - 1);
		const byte bitMaskAligned = ((bitMask << (8 - unitCount)) >> bitOffset);

		_allocationMeta[allocationMetaIndexOffset] ^= bitMaskAligned;
		unitCount = 0;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline bool StackHolder<UnitByteSize, MaxUnitCount>::canRegister(const byte unitOffset, const byte unitCount, uint32& outAllocationMetaIndex, byte& outAlignmentIndex, byte& outBitMask) const noexcept
	{
		const byte bitMask = (static_cast<byte>(pow(2, unitCount)) - 1);
		for (uint32 allocationMetaIndex = 0; allocationMetaIndex < kMetaDataSize; ++allocationMetaIndex)
		{
			const byte bitMaskPreAligned = bitMask << (8 - unitCount);
			for (byte alignmentIndex = 0; alignmentIndex <= (8 - unitCount); ++alignmentIndex)
			{
				const byte bitMaskAligned = bitMaskPreAligned >> alignmentIndex;
				const byte maskingResultClean0 = (_allocationMeta[allocationMetaIndex] ^ bitMaskAligned) << alignmentIndex;
				const byte maskingResultClean1 = maskingResultClean0 >> (8 - unitCount);
				const byte maskingResult = (maskingResultClean1 << (8 - unitCount)) >> alignmentIndex;
				if (maskingResult == bitMaskAligned)
				{
					outAllocationMetaIndex = allocationMetaIndex;
					outAlignmentIndex = alignmentIndex;
					outBitMask = bitMaskAligned;
					return true;
				}
			}
		}
		return false;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline bool StackHolder<UnitByteSize, MaxUnitCount>::canDeregister(const byte* const ptr, const byte unitCount) const noexcept
	{
		if (ptr < &_byteArray[0])
		{
			return false;
		}

		const uint32 rawOffset = ptr - &_byteArray[0];
		if (kRawDataSize <= rawOffset)
		{
			return false;
		}

		if (kRawDataSize < rawOffset + unitCount * UnitByteSize)
		{
			return false;
		}

		return true;
	}
}
