#pragma once


#ifndef MINT_CONTAINER_MEMORY_RAW_H
#define MINT_CONTAINER_MEMORY_RAW_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    namespace MemoryRaw
    {
#pragma region Type Traits
        template<typename T>
        constexpr const bool  isConstructible() noexcept;

        template<typename T>
        constexpr const bool  isDefaultConstructible() noexcept;

        template<typename T>
        constexpr const bool  isCopyConstructible() noexcept;

        template<typename T>
        constexpr const bool  isCopyAssignable() noexcept;

        template<typename T>
        constexpr const bool  isCopiable() noexcept;

        template<typename T>
        constexpr const bool  isMoveConstructible() noexcept;

        template<typename T>
        constexpr const bool  isMoveAssignable() noexcept;

        template<typename T>
        constexpr const bool  isMovable() noexcept;
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


#endif // !MINT_CONTAINER_MEMORY_RAW_H
