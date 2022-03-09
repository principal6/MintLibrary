#pragma once


#ifndef _MINT_CONTAINER_SCOPE_VECTOR_HPP_
#define _MINT_CONTAINER_SCOPE_VECTOR_HPP_


#include <MintContainer/Include/ScopeVector.h>


namespace mint
{
    template<typename T, const uint32 Capacity>
    inline ScopeVector<T, Capacity>::ScopeVector()
        : _array{}
        , _size{ 0 }
    {
        __noop;
    }

    template<typename T, uint32 Capacity>
    inline ScopeVector<T, Capacity>::~ScopeVector()
    {
        __noop;
    }

    template<typename T, uint32 Capacity>
    inline T& ScopeVector<T, Capacity>::operator[](const uint32 index)
    {
        MINT_ASSERT(index < _size, "������ ��� �����Դϴ�. [index: %d / size: %d]", index, _size);
        return _array[index];
    }

    template<typename T, uint32 Capacity>
    inline const T& ScopeVector<T, Capacity>::operator[](const uint32 index) const
    {
        MINT_ASSERT(index < _size, "������ ��� �����Դϴ�. [index: %d / size: %d]", index, _size);
        return _array[index];
    }

    template<typename T, uint32 Capacity>
    inline void ScopeVector<T, Capacity>::resize(const uint32 size)
    {
        MINT_ASSERT(size <= Capacity, "size �� Capacity ���� Ŀ�� �� �����ϴ�!", size, Capacity);
        if (size == _size)
        {
            return;
        }

        _size = size;
    }

    template<typename T, uint32 Capacity>
    inline void ScopeVector<T, Capacity>::push_back(const T& entry)
    {
        if (full())
        {
            MINT_ASSERT(false, "ScopeVector �� �̹� ���� ���ֽ��ϴ�!");
            return;
        }

        _array[_size] = entry;
        ++_size;
    }

    template<typename T, uint32 Capacity>
    inline void ScopeVector<T, Capacity>::push_back(T&& entry)
    {
        if (full())
        {
            MINT_ASSERT(false, "ScopeVector �� �̹� ���� ���ֽ��ϴ�!");
            return;
        }

        _array[_size] = entry;
        ++_size;
    }

    template<typename T, uint32 Capacity>
    inline void ScopeVector<T, Capacity>::pop_back()
    {
        if (empty())
        {
            MINT_ASSERT(false, "ScopeVector �� �̹� ��� �ֽ��ϴ�!");
            return;
        }

        --_size;
    }

    template<typename T, uint32 Capacity>
    inline void ScopeVector<T, Capacity>::clear()
    {
        while (empty() == false)
        {
            pop_back();
        }
    }

    template<typename T, uint32 Capacity>
    inline T& ScopeVector<T, Capacity>::front()
    {
        if (_size == 0)
        {
            MINT_NEVER;
        }
        return _array[0];
    }

    template<typename T, uint32 Capacity>
    inline const T& ScopeVector<T, Capacity>::front() const
    {
        if (_size == 0)
        {
            MINT_NEVER;
        }
        return _array[0];
    }

    template<typename T, uint32 Capacity>
    inline T& ScopeVector<T, Capacity>::back()
    {
        if (_size == 0)
        {
            MINT_NEVER;
        }
        return _array[_size - 1];
    }

    template<typename T, uint32 Capacity>
    inline const T& ScopeVector<T, Capacity>::back() const
    {
        if (_size == 0)
        {
            MINT_NEVER;
        }
        return _array[_size - 1];
    }

    template<typename T, uint32 Capacity>
    MINT_INLINE T& ScopeVector<T, Capacity>::at(const uint32 index) noexcept
    {
        MINT_ASSERT(index < _size, "������ ��� �����Դϴ�.");
        return _array[mint::min(index, _size - 1)];
    }

    template<typename T, uint32 Capacity>
    MINT_INLINE const T& ScopeVector<T, Capacity>::at(const uint32 index) const noexcept
    {
        MINT_ASSERT(index < _size, "������ ��� �����Դϴ�.");
        return _array[mint::min(index, _size - 1)];
    }
}


#endif // !_MINT_CONTAINER_SCOPE_VECTOR_HPP_
