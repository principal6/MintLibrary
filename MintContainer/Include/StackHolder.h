#pragma once


#ifndef _MINT_CONTAINER_STACK_HOLDER_H_
#define _MINT_CONTAINER_STACK_HOLDER_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Array.h>


namespace mint
{
    template<uint32 UnitByteSize, uint32 MaxUnitCount>
    class StackHolder final
    {
        using BitMaskType           = uint16;
        using CountMetaDataType     = uint8;

    public:
                                    StackHolder();
                                    StackHolder(const StackHolder& rhs) = delete;
                                    StackHolder(StackHolder&& rhs) = delete;
                                    ~StackHolder();

    public:
        StackHolder&                operator=(const StackHolder& rhs) = delete;
        StackHolder&                operator=(StackHolder&& rhs) = delete;

    public:
        byte*                       registerSpace(const CountMetaDataType unitCount);
        void                        deregisterSpace(byte*& ptr);

    private:
        bool                        canRegister(const CountMetaDataType unitCount, uint32& outAllocMetaDataIndex, uint8& outBitOffset, BitMaskType& outBitMask) const noexcept;
        bool                        canDeregister(const byte* const ptr, const CountMetaDataType unitCount) const noexcept;
        bool                        isInsider(const byte* const ptr) const noexcept;

    private:
        static constexpr uint32     kBitMaskByteCount = sizeof(BitMaskType) * kBitsPerByte;

        Array<CountMetaDataType, MaxUnitCount>  _allocCountDataArray;
        
        static constexpr uint32     kAllocMetaDataCount = ((MaxUnitCount - 1) / kBitMaskByteCount) + 1;
        
        Array<BitMaskType, kAllocMetaDataCount> _allocMetaDataArray;

        static constexpr uint32     kRawByteCount = UnitByteSize * MaxUnitCount;

        Array<byte, kRawByteCount>  _rawByteArray;
    };
}


#endif // !_MINT_CONTAINER_STACK_HOLDER_H_
