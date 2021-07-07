#pragma once


#ifndef MINT_COMMON_DEFINITIONS_H
#define MINT_COMMON_DEFINITIONS_H


#define MINT_INLINE __forceinline


#if defined(DEBUG) | defined(_DEBUG)
#define MINT_DEBUG
#endif


#include <cstdint>


namespace mint
{
    template<typename T>
    MINT_INLINE constexpr const T& max(const T& a, const T& b)
    {
        return (a < b) ? b : a;
    }

    template<typename T>
    MINT_INLINE constexpr const T& max(T&& a, T&& b)
    {
        return (a < b) ? b : a;
    }

    template<typename T>
    MINT_INLINE constexpr const T& min(const T& a, const T& b)
    {
        return (a > b) ? b : a;
    }

    template<typename T>
    MINT_INLINE constexpr const T& min(T&& a, T&& b)
    {
        return (a > b) ? b : a;
    }
}


#pragma region Integer definitions
    using                 int8                  =   int8_t;
    using                int16                  =  int16_t;
    using                int32                  =  int32_t;
    using                int64                  =  int64_t;

    using                uint8                  =  uint8_t;
    using               uint16                  = uint16_t;
    using               uint32                  = uint32_t;
    using               uint64                  = uint64_t;

    using                 byte                  =    uint8;

    static constexpr      int8 kInt8Max         = (  int8) 0x7F;
    static constexpr      int8 kInt8Min         = (  int8)~0x7F;
    static constexpr     int16 kInt16Max        = ( int16) 0x7FFF;
    static constexpr     int16 kInt16Min        = ( int16)~0x7FFF;
    static constexpr     int32 kInt32Max        = ( int32) 0x7FFFFFFF;
    static constexpr     int32 kInt32Min        = ( int32)~0x7FFFFFFF;
    static constexpr     int64 kInt64Max        = ( int64) 0x7FFFFFFFFFFFFFFF;
    static constexpr     int64 kInt64Min        = ( int64)~0x7FFFFFFFFFFFFFFF;
                                                
    static constexpr     uint8 kUint8Max        = ( uint8) 0xFF;
    static constexpr    uint16 kUint16Max       = (uint16) 0xFFFF;
    static constexpr    uint32 kUint32Max       = (uint32) 0xFFFFFFFF;
    static constexpr    uint64 kUint64Max       = (uint64) 0xFFFFFFFFFFFFFFFF;

    static constexpr    uint32 kBitsPerByte     = 8;
    static_assert(kBitsPerByte == 8, "Number of Bits per Byte must be 8!!!");

    static constexpr    uint32 kStackSizeLimit  = 1 << 14;
#pragma endregion


#pragma region String
    static constexpr    uint32 kStringNPos      = kUint32Max;
#pragma endregion


#pragma region File
    static constexpr    uint32 kMaxPath         = 260;
#pragma endregion


#ifdef MINT_CHECK_HEAP_ALLOCATION
    __noop;
#else
    #define MINT_NEW(type, ...) new type{ __VA_ARGS__ }
    #define MINT_PLACEMNT_NEW(pointer, ctor) new (pointer) ctor
    #define MINT_NEW_ARRAY(type, size) new type[size]{}
    #define MINT_NEW_ARRAY_USING_BYTE(type, size) reinterpret_cast<type*>(new byte[sizeof(type) * size]{})
    #define MINT_DELETE(obj) if (obj != nullptr) { delete obj; obj = nullptr; }
    #define MINT_DELETE_ARRAY(obj) if (obj != nullptr) { delete[] obj; obj = nullptr; }
    #define MINT_MALLOC(type, count) reinterpret_cast<type*>(::malloc(sizeof(type) * count))
    #define MINT_FREE(pointer) ::free(pointer); pointer = nullptr
#endif


    namespace mint
    {
        template<typename T>
        class Safe
        {
        public:
                                Safe() : _ptr{ nullptr } { __noop; }
                                Safe(T* const ptr) : _ptr{ ptr } { __noop; }
                                ~Safe() { release(); }

        public:
            T*                  operator&() const noexcept { return _ptr; }
            T&                  operator*() noexcept { return *_ptr; }
            T*                  operator->() noexcept { return _ptr; }

        public:
            void                assign(T* const ptr) { release(); _ptr = ptr; }
            void                release() { MINT_DELETE(_ptr); }
            constexpr uint64    size() { return sizeof(T); }

        private:
            T*                  _ptr;
        };
    }


#endif // !MINT_COMMON_DEFINITIONS_H
