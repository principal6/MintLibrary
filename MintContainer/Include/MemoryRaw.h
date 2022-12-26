#pragma once


#ifndef _MINT_CONTAINER_MEMORY_RAW_H_
#define _MINT_CONTAINER_MEMORY_RAW_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    namespace MemoryRaw
    {
#pragma region Type Traits
        template<typename T>
        constexpr bool  isConstructible() noexcept;

        template<typename T>
        constexpr bool  isDefaultConstructible() noexcept;

        template<typename T>
        constexpr bool  isCopyConstructible() noexcept;

        template<typename T>
        constexpr bool  isCopyAssignable() noexcept;

        template<typename T>
        constexpr bool  isCopiable() noexcept;

        template<typename T>
        constexpr bool  isMoveConstructible() noexcept;

        template<typename T>
        constexpr bool  isMoveAssignable() noexcept;

        template<typename T>
        constexpr bool  isMovable() noexcept;
#pragma endregion


        template<typename T>
        T*      allocateMemory(const uint32 size) noexcept;
        
        template<typename T>
        void    moveMemory(T* const to, const T* const from, const uint32 count) noexcept;

        template<typename T>
        void    deallocateMemory(T*& rawPointer) noexcept;


        template<typename T>
        void    construct(T& at) noexcept;

        template<typename T>
        void    copyAssign(T& to, const T& from) noexcept;

        template<typename T>
        void    moveAssign(T& to, T&& from) noexcept;

        template<typename T>
        void    copyConstruct(T& to, const T& from) noexcept;

        template<typename T>
        void    moveConstruct(T& to, T&& from) noexcept;

        template<typename T>
        void    destroy(T& at) noexcept;
    }
}


#endif // !_MINT_CONTAINER_MEMORY_RAW_H_
