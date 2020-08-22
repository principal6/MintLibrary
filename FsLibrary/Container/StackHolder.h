#pragma once


#ifndef FS_STACK_HOLDER_H
#define FS_STACK_HOLDER_H


#include <CommonDefinitions.h>
#include <Container/StaticArray.h>


namespace fs
{
	template<uint32 UnitByteSize, uint32 MaxUnitCount>
	class StackHolder final
	{
	public:
											StackHolder();
											StackHolder(const StackHolder& rhs) = delete;
											StackHolder(StackHolder&& rhs) = delete;
											~StackHolder();

	public:
		StackHolder&						operator=(const StackHolder& rhs) = delete;
		StackHolder&						operator=(StackHolder&& rhs) = delete;

	public:
		byte*								registerSpace(const byte unitCount);
		void								deregisterSpace(const byte* ptr);

	private:
		bool								isInsideHolder(const byte* ptr, const byte unitCount);

	private:
		StaticArray<byte, MaxUnitCount>		_allocationSizeArray;
		
		static constexpr uint32				kMetaDataSize = (MaxUnitCount / 8) + 1;
		
		StaticArray<byte, kMetaDataSize>	_allocationMeta;

		static constexpr uint32				kRawDataSize = UnitByteSize * MaxUnitCount;

		StaticArray<byte, kRawDataSize>		_byteArray;
	};
	
}


#endif // !FS_STACK_HOLDER_H
