#pragma once


#ifndef MINT_VECTOR_HPP
#define MINT_VECTOR_HPP


#include <type_traits>

#include <MintCommon/Include/Logger.h>

#include <MintContainer/Include/Vector.h>


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
        resize(count);

        const T* const first = initializerList.begin();
        for (uint32 index = 0; index < count; ++index)
        {
            _rawPointer[index] = *(first + index);
        }
    }

    template<typename T>
    inline Vector<T>::Vector(const Vector& rhs) noexcept
        : Vector()
    {
        resize(rhs._size);

        if constexpr (std::is_copy_assignable<T>::value == false)
        {
            for (uint32 index = 0; index < _size; ++index)
            {
                _rawPointer[index].~T();
            }
            for (uint32 index = 0; index < _size; ++index)
            {
                MINT_PLACEMNT_NEW(&_rawPointer[index], T(rhs._rawPointer[index]));
            }
        }
        else
        {
            for (uint32 index = 0; index < _size; ++index)
            {
                _rawPointer[index] = rhs._rawPointer[index];
            }
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
        deallocateMemoryInternal(_rawPointer, _size);
    }

    template<typename T>
    MINT_INLINE Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) noexcept
    {
        if (this != &rhs)
        {
            resize(rhs._size);

            for (uint32 index = 0; index < _size; ++index)
            {
                _rawPointer[index] = rhs._rawPointer[index];
            }
        }
        return *this;
    }

    template<typename T>
    MINT_INLINE Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept
    {
        if (this != &rhs)
        {
            deallocateMemoryInternal(_rawPointer, _size);

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
        MINT_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[index];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[index];
    }

    template<typename T>
    MINT_INLINE void Vector<T>::reserve(uint32 capacity) noexcept
    {
        if (capacity <= _capacity)
        {
            return;
        }

        // 잦은 reserve 시 성능 최적화!!!
        capacity = mint::max(capacity, _capacity * 2);

        T* temp = nullptr;
        if (0 < _size)
        {
            temp = allocateMemoryInternal(_size);

            for (uint32 index = 0; index < _size; ++index)
            {
                copyElementInternal(temp[index], _rawPointer[index], false);
            }
        }

        deallocateMemoryInternal(_rawPointer, _size);

        _rawPointer = allocateMemoryInternal(capacity);

        if (0 < _size)
        {
            for (uint32 index = 0; index < _size; ++index)
            {
                copyElementInternal(_rawPointer[index], temp[index], false);
            }
        }

        deallocateMemoryInternal(temp, _size);

        _capacity = capacity;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::resize(const uint32 size) noexcept
    {
        reserve(size);

        if constexpr (std::is_copy_constructible<T>::value == true)
        {
            if (_size < size)
            {
                for (uint32 index = _size; index < size; ++index)
                {
                    MINT_PLACEMNT_NEW(&_rawPointer[index], T());
                }
            }
            else if (size < _size)
            {
                for (uint32 index = size; index < _size; ++index)
                {
                    _rawPointer[index].~T();
                }
            }
        }
        else if constexpr (std::is_default_constructible<T>::value == true)
        {
            __noop;
        }
        else
        {
            static_assert(false, "What...");
        }

        _size = size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::shrink_to_fit() noexcept
    {
        if (_size < _capacity)
        {
            T* temp = allocateMemoryInternal(_size);
            for (uint32 index = 0; index < _size; ++index)
            {
                copyElementInternal(temp[index], _rawPointer[index], false);
            }

            deallocateMemoryInternal(_rawPointer, _size);
            _rawPointer = allocateMemoryInternal(_size);

            for (uint32 index = 0; index < _size; ++index)
            {
                copyElementInternal(_rawPointer[index], temp[index], false);
            }

            deallocateMemoryInternal(temp, _size);
            
            _capacity = _size;
        }
    }

    template<typename T>
    inline T* Vector<T>::allocateMemoryInternal(const uint32 size) noexcept
    {
        if constexpr (std::is_copy_constructible<T>::value == true)
        {
            return MINT_MALLOC(T, size);
        }
        else if constexpr (std::is_default_constructible<T>::value == true)
        {
            return MINT_NEW_ARRAY(T, size);
        }
        else
        {
            static_assert(false, "What...");
        }
        return nullptr;
    }

    template<typename T>
    inline void Vector<T>::deallocateMemoryInternal(T*& rawPointer, const uint32 size) noexcept
    {
        if (rawPointer == nullptr)
        {
            return;
        }

        if constexpr (std::is_copy_constructible<T>::value == true)
        {
            for (uint32 index = 0; index < size; ++index)
            {
                rawPointer[index].~T();
            }

            MINT_FREE(rawPointer);
        }
        else if constexpr (std::is_default_constructible<T>::value == true)
        {
            MINT_DELETE_ARRAY(rawPointer);
        }
        else
        {
            static_assert(false, "What...");
        }
    }

    template<typename T>
    MINT_INLINE void Vector<T>::copyElementInternal(T& to, const T& from, const bool destroyBeforePlacement) noexcept
    {
        if constexpr (std::is_copy_constructible<T>::value == true)
        {
            if (destroyBeforePlacement == true)
            {
                to.~T();
            }

            MINT_PLACEMNT_NEW(&to, T(from));
        }
        else if constexpr (std::is_default_constructible<T>::value == true)
        {
            to = from;
        }
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

        if constexpr (std::is_copy_constructible<T>::value == true)
        {
            MINT_PLACEMNT_NEW(&_rawPointer[_size], T(newEntry));
        }
        else if constexpr (std::is_default_constructible<T>::value == true)
        {
            _rawPointer[_size] = newEntry;
        }
        else
        {
            static_assert(false, "What...");
        }

        ++_size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::push_back(T&& newEntry) noexcept
    {
        expandCapacityIfNecessary();

        if constexpr (std::is_copy_constructible<T>::value == true)
        {
            MINT_PLACEMNT_NEW(&_rawPointer[_size], T(std::move(newEntry)));
        }
        else if constexpr (std::is_default_constructible<T>::value == true)
        {
            _rawPointer[_size] = std::move(newEntry);
        }
        else
        {
            static_assert(false, "What...");
        }

        ++_size;
    }

    template<typename T>
    MINT_INLINE void Vector<T>::pop_back() noexcept
    {
        if (_size == 0)
        {
            return;
        }
        
        if constexpr (std::is_copy_constructible<T>::value == true)
        {
            _rawPointer[_size - 1].~T();
        }
        else if constexpr (std::is_default_constructible<T>::value == true)
        {
            __noop;
        }
        else
        {
            static_assert(false, "What...");
        }
        

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

            for (uint32 iter = _size; iter > at; --iter)
            {
                _rawPointer[iter] = std::move(_rawPointer[iter - 1]);
            }
            _rawPointer[at] = newEntry;

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
            const uint32 beginIndex = at + 1;
            for (uint32 iter = beginIndex; iter < _size; ++iter)
            {
                _rawPointer[iter - 1] = std::move(_rawPointer[iter]);
            }

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
        MINT_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[0];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::front() const noexcept
    {
        MINT_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[0];
    }

    template<typename T>
    MINT_INLINE T& Vector<T>::back() noexcept
    {
        MINT_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[_size - 1];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::back() const noexcept
    {
        MINT_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[_size - 1];
    }

    template<typename T>
    MINT_INLINE T& Vector<T>::at(const uint32 index) noexcept
    {
        MINT_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[std::min(index, _size - 1)];
    }

    template<typename T>
    MINT_INLINE const T& Vector<T>::at(const uint32 index) const noexcept
    {
        MINT_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
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
