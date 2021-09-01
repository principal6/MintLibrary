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


        // ctor �� ȣ������ �ʰ� �޸𸮸� �Ҵ��Ѵ�.
        // �� �Լ� ȣ�� ���� ���������� ctor �� ȣ���ؾ� �Ѵ�!
        template<typename T>
        MINT_INLINE T* allocateMemory(const uint32 size) noexcept
        {
            if constexpr (isConstructible<T>() == true)
            {
                return MINT_MALLOC(T, size);
            }
            else
            {
                static_assert(false, "Not constructible type!!!");
            }
            return nullptr;
        }

        template<typename T>
        // memory �� move �Ѵ�. (to �� from �� �޸𸮰� ��ġ���� �����ϰ� �ű��)
        // move ctor, copy ctor �Ǵ� move assignment, copy assignment �� ȣ����� �ʴ´�!!!
        MINT_INLINE void moveMemory(T* const to, const T* const from, const uint32 count) noexcept
        {
            std::memmove(to, from, sizeof(T) * count);
        }

        template<typename T>
        // dtor �� ȣ������ �ʰ� �޸𸮸� �����Ѵ�!!
        // �� �Լ� ȣ�� ���� ���������� dtor �� ȣ���߾�� �Ѵ�!
        MINT_INLINE void deallocateMemory(T*& rawPointer) noexcept
        {
            if (rawPointer == nullptr)
            {
                return;
            }

            MINT_FREE(rawPointer);
        }

        template<typename T>
        // at �� ����ִ� instance ��� �� �Լ� ȣ�� ���� �ݵ�� dtor �� ���������� ȣ���߾�� �Ѵ�!
        MINT_INLINE void construct(T& at) noexcept
        {
            if constexpr (isDefaultConstructible<T>() == true)
            {
                MINT_PLACEMNT_NEW(&at, T());
            }
            else
            {
                static_assert("Not default-constructible!!!");
            }
        }

        template<typename T>
        // to �� ����ִ� instance ��� �ݵ�� dtor �� ���������� ȣ���ؾ� �Ѵ�!
        MINT_INLINE void copyConstructRaw(T& to, const T& from) noexcept
        {
            MINT_PLACEMNT_NEW(&to, T(from));
        }

        template<typename T>
        // to �� ����ִ� instance ��� �ݵ�� dtor �� ���������� ȣ���ؾ� �Ѵ�!
        MINT_INLINE void moveConstructRaw(T& to, T&& from) noexcept
        {
            MINT_PLACEMNT_NEW(&to, T(std::move(from)));
        }

        template<typename T>
        // to �� ����ִ� instance ���߸� �Ѵ�!
        MINT_INLINE void copyAssign(T& to, const T& from) noexcept
        {
            if constexpr (isCopyAssignable<T>() == true)
            {
                to = from;
            }
            else
            {
                static_assert(false, "Not copy-assignable!!!");
            }
            
        }

        template<typename T>
        // to �� ����ִ� instance ���߸� �Ѵ�!
        MINT_INLINE void moveAssign(T& to, T&& from) noexcept
        {
            if constexpr (isMoveAssignable<T>() == true)
            {
                to = std::move(from);
            }
            else
            {
                static_assert(false, "Not move-assignable!!!");
            }
        }

        template<typename T>
        // ctor �� �ݵ�� ȣ��Ǵ� ���� �����Ѵ�.
        // to �� ����ִ� instance ��� �� �Լ� ȣ�� ���� �ݵ�� dtor �� ���������� ȣ���߾�� �Ѵ�!
        MINT_INLINE void copyConstruct(T& to, const T& from) noexcept
        {
            if constexpr (isCopyConstructible<T>() == true)
            {
                copyConstructRaw<T>(to, from);
            }
            else if constexpr (isDefaultConstructible<T>() == true && isCopyAssignable<T>() == true)
            {
                construct<T>(to);

                to = from;
            }
            else
            {
                static_assert(false, "Not copiable!!!");
            }
        }

        template<typename T>
        // ctor �� �ݵ�� ȣ��Ǵ� ���� �����Ѵ�.
        // to �� ����ִ� instance ��� �� �Լ� ȣ�� ���� �ݵ�� dtor �� ���������� ȣ���߾�� �Ѵ�!
        MINT_INLINE void moveConstruct(T& to, T&& from) noexcept
        {
            if constexpr (isMoveConstructible<T>() == true)
            {
                moveConstructRaw<T>(to, std::move(from));
            }
            else if constexpr (isDefaultConstructible<T>() == true && isMoveAssignable<T>() == true)
            {
                construct<T>(to);

                to = std::move(from);
            }
            else
            {
                static_assert(false, "Not movable!!!");
            }
        }

        template<typename T>
        // at �� ����ִ� instance ���߸� �Ѵ�!
        MINT_INLINE void destroy(T& at) noexcept
        {
            at.~T();
        }
    }
}


#endif // !MINT_CONTAINER_MEMORY_RAW_HPP