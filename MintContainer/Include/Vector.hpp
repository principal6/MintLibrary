#pragma once


#ifndef MINT_VECTOR_HPP
#define MINT_VECTOR_HPP


#include <initializer_list>

#include <MintCommon/Include/Logger.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/MemoryRaw.hpp>


namespace mint
{
    template <typename T>
    inline Vector<T>::Vector()
        : _rawPointer{ nullptr }
        , _capacity{ 0 }
        , _size{ 0 }
    {
        __noop;
    }

    template<typename T>
    inline Vector<T>::Vector(const uint32 size)
        : Vector()
    {
        resize(size);
    }

    template<typename T>
    inline Vector<T>::Vector(const std::initializer_list<T>& initializerList)
        : Vector()
    {
        const uint32 count = static_cast<uint32>(initializerList.size());
        reserve(count);
        
        const T* const rawPointer = initializerList.begin();
        for (uint32 index = 0; index < count; ++index)
        {
            push_back(rawPointer[index]);
        }
    }

    template<typename T>
    inline Vector<T>::Vector(const Vector& rhs) noexcept
        : Vector()
    {
        reserve(rhs._size);

        for (uint32 index = 0; index < rhs._size; ++index)
        {
            push_back(rhs._rawPointer[index]);
        }
    }

    template<typename T>
    inline Vector<T>::Vector(Vector&& rhs) noexcept
        : _rawPointer{ rhs._rawPointer }
        , _capacity{ std::move(rhs._capacity) }
        , _size{ std::move(rhs._size) }
    {
        rhs._rawPointer = nullptr;
    }

    template<typename T>
    inline Vector<T>::~Vector()
    {
        clear();

        MemoryRaw::deallocateMemory<T>(_rawPointer);
    }

    template<typename T>
    MINT_INLINE Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();

            reserve(rhs._size);

            for (uint32 index = 0; index < rhs._size; ++index)
            {
                push_back(rhs._rawPointer[index]);
            }
        }
        return *this;
    }

    template<typename T>
    MINT_INLINE Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();

            MemoryRaw::deallocateMemory<T>(_rawPointer);

            _rawPointer = rhs._rawPointer;
            _capacity = std::move(rhs._capacity);
            _size = std::move(rhs._size);

            rhs._rawPointer = nullptr;
        }
        return *this;
    }

    template<typename T>
    MINT_INLINE T& Vector<T>::operator[](const uint32 index) noexcept
    {
        MINT_ASSERT("�����", index < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[index];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT("�����", index < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[index];
    }

    template<typename T>
    MINT_INLINE void Vector<T>::reserve(const uint32 capacity) noexcept
    {
        if (capacity <= _capacity)
        {
            return;
        }

        // ���� reserve �� ���� ����ȭ!!!
        _capacity = mint::max(capacity, _capacity * 2);

        if (_size == 0)
        {
            MemoryRaw::deallocateMemory<T>(_rawPointer);
            _rawPointer = MemoryRaw::allocateMemory<T>(_capacity);
        }
        else
        {
            T* temp = MemoryRaw::allocateMemory<T>(_size);
            MemoryRaw::moveMemory<T>(temp, _rawPointer, _size);

            MemoryRaw::deallocateMemory<T>(_rawPointer);
            _rawPointer = MemoryRaw::allocateMemory<T>(_capacity);

            MemoryRaw::moveMemory<T>(_rawPointer, temp, _size);
            MemoryRaw::deallocateMemory<T>(temp);
        }
    }

    template<typename T>
    MINT_INLINE void Vector<T>::resize(const uint32 size) noexcept
    {
        reserve(size);

        if (_size < size)
        {
            for (uint32 index = _size; index < size; ++index)
            {
                MemoryRaw::construct(_rawPointer[index]);
            }
        }
        else if (size < _size)
        {
            for (uint32 index = size; index < _size; ++index)
            {
                pop_back();
            }
        }

        _size = size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::shrink_to_fit() noexcept
    {
        if (_capacity <= _size)
        {
            return;
        }

        T* temp = MemoryRaw::allocateMemory<T>(_size);
        MemoryRaw::moveMemory<T>(temp, _rawPointer, _size);

        MemoryRaw::deallocateMemory<T>(_rawPointer);
        _rawPointer = MemoryRaw::allocateMemory<T>(_size);

        MemoryRaw::moveMemory<T>(_rawPointer, temp, _size);
        MemoryRaw::deallocateMemory<T>(temp);

        _capacity = _size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::clear() noexcept
    {
        while (empty() == false)
        {
            pop_back();
        }
    }

    template<typename T>
    MINT_INLINE void Vector<T>::push_back(const T& newEntry) noexcept
    {
        expandCapacityIfNecessary();

        MemoryRaw::copyConstruct<T>(_rawPointer[_size], newEntry);
        
        ++_size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::push_back(T&& newEntry) noexcept
    {
        expandCapacityIfNecessary();

        MemoryRaw::moveConstruct<T>(_rawPointer[_size], std::move(newEntry));
        
        ++_size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::pop_back() noexcept
    {
        if (_size == 0)
        {
            return;
        }
        
        // valid �� ���� �� [0, _size - 1] element ��
        // ��� ctor �� ȣ���� ����Ǿ����Ƿ�
        // �ݵ�� destroy() �� ȣ���ؾ� �Ѵ�.
        MemoryRaw::destroy<T>(_rawPointer[_size - 1]);
        
        --_size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::insert(const T& newEntry, const uint32 at) noexcept
    {
        if (_size <= at)
        {
            push_back(newEntry);
        }
        else
        {
            expandCapacityIfNecessary();

            if constexpr (MemoryRaw::isMovable<T>() == true)
            {
                MemoryRaw::moveConstruct<T>(_rawPointer[_size], std::move(_rawPointer[_size - 1]));

                for (uint32 iter = _size - 1; iter > at; --iter)
                {
                    MemoryRaw::moveAssign<T>(_rawPointer[iter], std::move(_rawPointer[iter - 1]));
                }
            }
            else // ��ȿ���������� ������ �ϵ��� �Ѵ�.
            {
                MemoryRaw::copyConstruct<T>(_rawPointer[_size], _rawPointer[_size - 1]);

                for (uint32 iter = _size - 1; iter > at; --iter)
                {
                    MemoryRaw::copyAssign<T>(_rawPointer[iter], _rawPointer[iter - 1]);
                }
            }

            MemoryRaw::copyAssign<T>(_rawPointer[at], newEntry);

            ++_size;
        }
    }

    template<typename T>
    MINT_INLINE void Vector<T>::insert(T&& newEntry, const uint32 at) noexcept
    {
        if (_size <= at)
        {
            push_back(newEntry);
        }
        else
        {
            expandCapacityIfNecessary();

            if constexpr (MemoryRaw::isMovable<T>() == true)
            {
                MemoryRaw::moveConstruct<T>(_rawPointer[_size], std::move(_rawPointer[_size - 1]));

                for (uint32 iter = _size - 1; iter > at; --iter)
                {
                    MemoryRaw::moveAssign<T>(_rawPointer[iter], std::move(_rawPointer[iter - 1]));
                }

                MemoryRaw::moveAssign<T>(_rawPointer[at], std::move(newEntry));
            }
            else // ��ȿ���������� ������ �ϵ��� �Ѵ�.
            {
                MemoryRaw::copyConstruct<T>(_rawPointer[_size], _rawPointer[_size - 1]);
                
                for (uint32 iter = _size - 1; iter > at; --iter)
                {
                    MemoryRaw::copyAssign<T>(_rawPointer[iter], _rawPointer[iter - 1]);
                }
                
                MemoryRaw::copyAssign<T>(_rawPointer[at], newEntry);
            }

            ++_size;
        }
    }

    template<typename T>
    MINT_INLINE void Vector<T>::erase(const uint32 at) noexcept
    {
        if (_size == 0)
        {
            return;
        }

        if (at == _size - 1)
        {
            pop_back();
        }
        else
        {
            if constexpr (MemoryRaw::isMovable<T>() == true)
            {
                for (uint32 iter = at + 1; iter < _size; ++iter)
                {
                    MemoryRaw::moveAssign<T>(_rawPointer[iter - 1], std::move(_rawPointer[iter]));
                }
            }
            else // ��ȿ���������� ������ �ϵ��� �Ѵ�.
            {
                for (uint32 iter = at + 1; iter < _size; ++iter)
                {
                    MemoryRaw::copyAssign<T>(_rawPointer[iter - 1], _rawPointer[iter]);
                }
            }

            MemoryRaw::destroy<T>(_rawPointer[_size - 1]);

            --_size;
        }
    }

    template<typename T>
    MINT_INLINE void Vector<T>::expandCapacityIfNecessary() noexcept
    {
        if (_size == _capacity)
        {
            reserve((0 == _capacity) ? kBaseCapacity : _capacity * 2);
        }
    }

    template<typename T>
    MINT_INLINE T& Vector<T>::front() noexcept
    {
        MINT_ASSERT("�����", 0 < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[0];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::front() const noexcept
    {
        MINT_ASSERT("�����", 0 < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[0];
    }

    template<typename T>
    MINT_INLINE T& Vector<T>::back() noexcept
    {
        MINT_ASSERT("�����", 0 < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[_size - 1];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::back() const noexcept
    {
        MINT_ASSERT("�����", 0 < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[_size - 1];
    }

    template<typename T>
    MINT_INLINE T& Vector<T>::at(const uint32 index) noexcept
    {
        MINT_ASSERT("�����", index < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[std::min(index, _size - 1)];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::at(const uint32 index) const noexcept
    {
        MINT_ASSERT("�����", index < _size, "������ ��� �����Դϴ�.");
        return _rawPointer[std::min(index, _size - 1)];
    }

    template<typename T>
    MINT_INLINE const T* Vector<T>::data() const noexcept
    {
        return _rawPointer;
    }

    template<typename T>
    MINT_INLINE const uint32 Vector<T>::capacity() const noexcept
    {
        return _capacity;
    }

    template<typename T>
    MINT_INLINE const uint32 Vector<T>::size() const noexcept
    {
        return _size;
    }

    template<typename T>
    MINT_INLINE const bool Vector<T>::empty() const noexcept
    {
        return (_size == 0);
    }
}


#endif // !MINT_VECTOR_HPP
