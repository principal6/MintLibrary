#pragma once


#ifndef FS_STACK_HOLDER_H
#define FS_STACK_HOLDER_H


#include <CommonDefinitions.h>
#include <FsContainer/Include/StaticArray.h>


namespace fs
{
	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	class StackHolder final
	{
		using BitMaskType								= uint16;
		using CountMetaDataType							= uint8;

	public:
														StackHolder();
														StackHolder(const StackHolder& rhs) = delete;
														StackHolder(StackHolder&& rhs) = delete;
														~StackHolder();

	public:
		StackHolder&									operator=(const StackHolder& rhs) = delete;
		StackHolder&									operator=(StackHolder&& rhs) = delete;

	public:
		byte*											registerSpace(const CountMetaDataType unitCount);
		void											deregisterSpace(byte*& ptr);

	private:
		const bool										canRegister(const CountMetaDataType unitCount, uint32& outAllocMetaDataIndex, uint8& outBitOffset, BitMaskType& outBitMask) const noexcept;
		const bool										canDeregister(const byte* const ptr, const CountMetaDataType unitCount) const noexcept;
		const bool										isInsider(const byte* const ptr) const noexcept;

	private:
		static constexpr uint32							kBitMaskByteCount = sizeof(BitMaskType) * kBitsPerByte;

		StaticArray<CountMetaDataType, MaxUnitCount>	_allocCountDataArray;
		
		static constexpr uint32							kAllocMetaDataCount = ((MaxUnitCount - 1) / kBitMaskByteCount) + 1;
		
		StaticArray<BitMaskType, kAllocMetaDataCount>	_allocMetaDataArray;

		static constexpr uint32							kRawByteCount = UnitByteSize * MaxUnitCount;

		StaticArray<byte, kRawByteCount>				_rawByteArray;
	};
}


#endif // !FS_STACK_HOLDER_H
