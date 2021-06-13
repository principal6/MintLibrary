#pragma once


#ifndef MINT_STATIC_BIT_ARRAY_H
#define MINT_STATIC_BIT_ARRAY_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    template <uint32 BitCount>
    class StaticBitArray
    {
    public:
                                    StaticBitArray();
                                    StaticBitArray(const bool initialValue);
                                    ~StaticBitArray();

    public:
        void                        setAll(const bool value) noexcept;
    
    public:
        void                        setByte(const uint32 byteAt, const byte value) noexcept;
        void                        setByteUnsafe(const uint32 byteAt, const byte value) noexcept;

    public:
        const byte                  getByte(const uint32 byteAt) const noexcept;
        const byte                  getByteUnsafe(const uint32 byteAt) const noexcept;

    public:
        void                        set(const uint32 bitAt, const bool value) noexcept;
        void                        setUnsafe(const uint32 bitAt, const bool value) noexcept;
        void                        set(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept;
        void                        setUnsafe(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept;

    public:
        const bool                  get(const uint32 bitAt) const noexcept;
        const bool                  getUnsafe(const uint32 bitAt) const noexcept;
        const bool                  get(const uint32 byteAt, const uint8 bitOffset) const noexcept;
        const bool                  getUnsafe(const uint32 byteAt, const uint8 bitOffset) const noexcept;

    public:
        const uint32                getBitCount() const noexcept;
        const uint32                getByteCount() const noexcept;

    private:
        static constexpr uint32     kByteCount{ 1 + ((BitCount - 1) / kBitsPerByte) };

    private:
        byte                        _byteArray[kByteCount];
    };
}


#endif // !MINT_STATIC_BIT_ARRAY_H
