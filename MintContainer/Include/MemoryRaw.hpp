#pragma once


#ifndef MINT_CONTAINER_MEMORY_RAW_HPP
#define MINT_CONTAINER_MEMORY_RAW_HPP


#include <MintContainer/Include/MemoryRaw.h>

#include <type_traits>


namespace mint
{
    namespace MemoryRaw
    {
#pragma region Type Traits
        template<typename T>
        MINT_INLINE constexpr const bool isConstructible() noexcept
        {
            return (isDefaultConstructible<T>() || isCopyConstructible<T>() || isMoveConstructible<T>());
        }

        template<typename T>
        MINT_INLINE constexpr const bool isDefaultConstructible() noexcept
        {
            return (std::is_default_constructible<T>::value == true);
        }

        template<typename T>
        MINT_INLINE constexpr const bool isCopyConstructible() noexcept
        {
            return (std::is_copy_constructible<T>::value == true);
        }

        template<typename T>
        MINT_INLINE constexpr const bool isCopyAssignable() noexcept
        {
            return (std::is_copy_assignable<T>::value == true);
        }

        template<typename T>
        MINT_INLINE constexpr const bool isCopiable() noexcept
        {
            return (isCopyConstructible<T>() || isCopyAssignable<T>());
        }

        template<typename T>
        MINT_INLINE constexpr const bool isMoveConstructible() noexcept
        {
            return (std::is_move_constructible<T>::value == true);
        }

        template<typename T>
        MINT_INLINE constexpr const bool isMoveAssignable() noexcept
        {
            return (std::is_move_assignable<T>::value == true);
        }

        template<typename T>
        MINT_INLINE constexpr const bool isMovable() noexcept
        {
            return (isMoveConstructible<T>() || isMoveAssignable<T>());
        }
#pragma endregion


        // ctor 를 호출하지 않고 메모리만 할당한다.
        // 이 함수 호출 이후 명시적으로 ctor 를 호출해야 한다!
        template<typename T>
        MINT_INLINE T* allocateMemory(const uint32 size) noexcept
        {
            static_assert(isConstructible<T>(), "Not constructible type!!!");

            return MINT_MALLOC(T, size);
        }

        template<typename T>
        // memory 를 move 한다. (to 와 from 의 메모리가 겹치더라도 안전하게 옮긴다)
        // move ctor, copy ctor 또는 move assignment, copy assignment 가 호출되지 않는다!!!
        MINT_INLINE void moveMemory(T* const to, const T* const from, const uint32 count) noexcept
        {
            std::memmove(to, from, sizeof(T) * count);
        }

        template<typename T>
        // dtor 를 호출하지 않고 메모리만 해제한다!!
        // 이 함수 호출 전에 명시적으로 dtor 를 호출했어야 한다!
        MINT_INLINE void deallocateMemory(T*& rawPointer) noexcept
        {
            if (rawPointer == nullptr)
            {
                return;
            }

            MINT_FREE(rawPointer);
        }

        template<typename T>
        // at 가 살아있는 instance 라면 이 함수 호출 전에 반드시 dtor 를 명시적으로 호출했어야 한다!
        MINT_INLINE void construct(T& at) noexcept
        {
            static_assert(isDefaultConstructible<T>(), "Not default-constructible!!!");
            
            MINT_PLACEMNT_NEW(&at, T());
        }

        template<typename T>
        // to 가 살아있는 instance 라면 반드시 dtor 를 명시적으로 호출해야 한다!
        MINT_INLINE void copyConstructRaw(T& to, const T& from) noexcept
        {
            MINT_PLACEMNT_NEW(&to, T(from));
        }

        template<typename T>
        // to 가 살아있는 instance 라면 반드시 dtor 를 명시적으로 호출해야 한다!
        MINT_INLINE void moveConstructRaw(T& to, T&& from) noexcept
        {
            MINT_PLACEMNT_NEW(&to, T(std::move(from)));
        }

        template<typename T>
        // to 가 살아있는 instance 여야만 한다!
        MINT_INLINE void copyAssign(T& to, const T& from) noexcept
        {
            static_assert(isCopyAssignable<T>(), "Not copy-assignable!!!");

            to = from;
        }

        template<typename T>
        // to 가 살아있는 instance 여야만 한다!
        MINT_INLINE void moveAssign(T& to, T&& from) noexcept
        {
            static_assert(isMoveAssignable<T>(), "Not move-assignable!!!");

            to = std::move(from);
        }

        template<typename T>
        // ctor 가 반드시 호출되는 것을 보장한다.
        // to 가 살아있는 instance 라면 이 함수 호출 전에 반드시 dtor 를 명시적으로 호출했어야 한다!
        MINT_INLINE void copyConstruct(T& to, const T& from) noexcept
        {
            static_assert(isCopyConstructible<T>() || (isDefaultConstructible<T>() == true && isCopyAssignable<T>() == true), "Not copiable!!!");
            
            if constexpr (isCopyConstructible<T>() == true)
            {
                copyConstructRaw<T>(to, from);
            }
            else if constexpr (isDefaultConstructible<T>() == true && isCopyAssignable<T>() == true)
            {
                construct<T>(to);

                to = from;
            }
        }

        template<typename T>
        // ctor 가 반드시 호출되는 것을 보장한다.
        // to 가 살아있는 instance 라면 이 함수 호출 전에 반드시 dtor 를 명시적으로 호출했어야 한다!
        MINT_INLINE void moveConstruct(T& to, T&& from) noexcept
        {
            static_assert(isMoveConstructible<T>() || (isDefaultConstructible<T>() == true && isMoveAssignable<T>() == true), "Not movable!!!");

            if constexpr (isMoveConstructible<T>() == true)
            {
                moveConstructRaw<T>(to, std::move(from));
            }
            else if constexpr (isDefaultConstructible<T>() == true && isMoveAssignable<T>() == true)
            {
                construct<T>(to);

                to = std::move(from);
            }
        }

        template<typename T>
        // at 이 살아있는 instance 여야만 한다!
        MINT_INLINE void destroy(T& at) noexcept
        {
            at.~T();
        }
    }
}


#endif // !MINT_CONTAINER_MEMORY_RAW_HPP
