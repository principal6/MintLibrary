﻿#pragma once


#ifndef MINT_BIT_VECTOR_H
#define MINT_BIT_VECTOR_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    class BitVector final
    {
        static constexpr uint32     kMinByteCapacity    = 1;

    public:
                                    BitVector();
                                    BitVector(const uint32 byteCapacity);
                                    ~BitVector();

    public:
        void                        push_back(const bool value);
        const bool                  pop_back();
        void                        resizeBitCount(const uint32 newBitCount);
        void                        reserveByteCapacity(const uint32 newByteCapacity);

    public:
        const bool                  isEmpty() const noexcept;
        const bool                  isFull() const noexcept;
        const bool                  isInSizeBoundary(const uint32 bitAt) const noexcept;
        const bool                  get(const uint32 bitAt) const noexcept;
        const uint8                 getByte(const uint32 byteAt) const noexcept;
        const bool                  first() const noexcept;
        const bool                  last() const noexcept;
        void                        set(const uint32 bitAt, const bool value) noexcept;
        void                        set(const uint32 byteAt, const uint32 bitOffsetFromLeft, const bool value) noexcept;
        void                        setByte(const uint32 byteAt, const uint8 byte) noexcept;
        void                        fill(const bool value) noexcept;
        void                        swap(const uint32 aBitAt, const uint32 bBitAt) noexcept;

    public:
        const uint32                bitCount() const noexcept;
        const uint32                byteCapacity() const noexcept;

    public:
        // 여러 비트를 set, get 할 때는 아래 함수들을 이용하는 게 성능에 훨씬 좋다!
        static void                 setBit(uint8& inOutByte, const uint32 bitOffsetFromLeft, const bool value) noexcept;
        static const bool           getBit(const uint8 byte, const uint32 bitOffsetFromLeft) noexcept;
    
    public:
        static const uint32         computeByteCount(const uint32 bitCount) noexcept;
        static const uint32         computeByteAt(const uint32 bitAt) noexcept;
        static const uint32         computeBitOffset(const uint32 bitAt) noexcept;
        static const uint8          makeByte(const bool (&valueArray)[8]) noexcept;
        static const uint8          makeBitMaskOneAt(const uint32 bitOffsetFromLeft) noexcept;

    private:
        uint8*                      _byteArray;
        uint32                      _byteCapacity;
        uint32                      _bitCount;
    };
}


#endif // !MINT_BIT_VECTOR_H
