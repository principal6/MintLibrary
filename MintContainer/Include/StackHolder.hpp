#pragma once


#include <MintContainer/Include/StackHolder.h>

#include <MintMath/Include/MathCommon.h>


namespace mint
{
	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline StackHolder<UnitByteSize, MaxUnitCount>::StackHolder()
		: _allocCountDataArray{}
		, _allocMetaDataArray{}
		, _rawByteArray{}
	{
		static_assert(UnitByteSize > 0, "UnitByteSize 를 1 이상의 값으로 지정하세요.");
		static_assert(MaxUnitCount > 0, "MaxUnitCount 를 1 이상의 값으로 지정하세요.");

		if ((MaxUnitCount % kBitMaskByteCount) != 0)
		{
			constexpr uint32 deleteBitCount = kBitMaskByteCount - (MaxUnitCount % kBitMaskByteCount);
			constexpr BitMaskType deleteBitMask = static_cast<BitMaskType>(Math::pow2_ui32(deleteBitCount) - 1);
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
		MINT_ASSERT(unitCount > 0, "!!! 0 개의 Unit 을 할당할 수는 없습니다 !!!");
		MINT_ASSERT(unitCount <= kBitMaskByteCount, "!!! 한번에 할당 가능한 최대 Unit 수를 넘었습니다 !!!");

		uint32 allocMetaDataIndex = 0;
		uint8 bitOffset = 0;
		BitMaskType bitMask = 0;
		if (canRegister(unitCount, allocMetaDataIndex, bitOffset, bitMask) == true)
		{
			// 할당 가능 !!!
			const uint32 unitIndex = (allocMetaDataIndex * kBitMaskByteCount + bitOffset);
			const uint32 byteOffset = unitIndex * UnitByteSize;
			_allocCountDataArray[unitIndex] = unitCount;
			_allocMetaDataArray[allocMetaDataIndex] |= bitMask;
			return &_rawByteArray[byteOffset];
		}
		MINT_ASSERT(false, "!!! StackHolder 가 가득 찼습니다 !!! 할당 실패 !!!");
		return nullptr;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline void StackHolder<UnitByteSize, MaxUnitCount>::deregisterSpace(byte*& ptr)
	{
		if (isInsider(ptr) == false)
		{
			MINT_ASSERT(false, "!!! Insider 가 아닙니다 !!!");
			return;
		}

		const int32 rawByteOffset = static_cast<int32>(ptr - &_rawByteArray[0]);
		const CountMetaDataType allocSizeDataIndex = rawByteOffset / UnitByteSize;
		CountMetaDataType& unitCount = _allocCountDataArray[allocSizeDataIndex];
		if (unitCount == 0)
		{
			MINT_ASSERT(false, "!!! 할당되지 않은 ptr 을 지울 수 없습니다 !!!");
			return;
		}

		if (canDeregister(ptr, unitCount) == false)
		{
			MINT_ASSERT(false, "!!! 이 StackHolder 에 할당된 ptr 이 아닙니다 !!!");
			return;
		}

		const uint32 allocMetaDataIndex = allocSizeDataIndex / kBitMaskByteCount;
		const uint8 bitOffset = allocSizeDataIndex % kBitMaskByteCount;
		const BitMaskType bitMask = static_cast<BitMaskType>(Math::pow2_ui32(unitCount) - 1);
		const BitMaskType bitMaskAligned = ((bitMask << (kBitMaskByteCount - unitCount)) >> bitOffset);

		_allocMetaDataArray[allocMetaDataIndex] ^= bitMaskAligned;
		unitCount = 0;
		ptr = nullptr;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline bool StackHolder<UnitByteSize, MaxUnitCount>::canRegister(const CountMetaDataType unitCount, uint32& outAllocMetaDataIndex, uint8& outBitOffset, BitMaskType& outBitMask) const noexcept
	{
		const BitMaskType bitMaskRightAligned = static_cast<BitMaskType>(Math::pow2_ui32(unitCount) - 1);
		const BitMaskType bitMaskLeftAligned = bitMaskRightAligned << (kBitMaskByteCount - unitCount);
		for (uint32 allocMetaDataIndex = 0; allocMetaDataIndex < kAllocMetaDataCount; ++allocMetaDataIndex)
		{
			for (uint8 bitOffset = 0; bitOffset <= (kBitMaskByteCount - unitCount); ++bitOffset)
			{
				const BitMaskType offsetBitMask = bitMaskLeftAligned >> bitOffset;
				const BitMaskType maskingProcess0 = (_allocMetaDataArray[allocMetaDataIndex] ^ offsetBitMask) << bitOffset;
				const BitMaskType maskingProcess1 = maskingProcess0 >> (kBitMaskByteCount - unitCount);
				const BitMaskType maskingResult = (maskingProcess1 << (kBitMaskByteCount - unitCount)) >> bitOffset;
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
		if (isInsider(ptr) == false)
		{
			return false;
		}

		const uint32 unitByteCount = static_cast<uint32>(unitCount * UnitByteSize);
		const uint32 rawByteOffset = static_cast<uint32>(ptr - &_rawByteArray[0]);
		if (kRawByteCount < rawByteOffset + unitByteCount)
		{
			return false;
		}

		return true;
	}

	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	inline bool StackHolder<UnitByteSize, MaxUnitCount>::isInsider(const byte* const ptr) const noexcept
	{
		if (ptr == nullptr)
		{
			return false;
		}

		if (ptr < &_rawByteArray[0])
		{
			return false;
		}

		const uint32 rawByteOffset = static_cast<uint32>(ptr - &_rawByteArray[0]);
		if (kRawByteCount <= rawByteOffset)
		{
			return false;
		}

		return true;
	}
}
