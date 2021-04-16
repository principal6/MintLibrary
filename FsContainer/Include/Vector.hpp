#pragma once


#ifndef FS_VECTOR_HPP
#define FS_VECTOR_HPP


#include <FsContainer/Include/Vector.h>


namespace fs
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

        for (uint32 index = 0; index < _size; ++index)
        {
            _rawPointer[index] = rhs._rawPointer[index];
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
        freeRawPointer(_rawPointer, _size);
    }

    template<typename T>
    FS_INLINE Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) noexcept
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
    FS_INLINE Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept
    {
        if (this != &rhs)
        {
            freeRawPointer(_rawPointer, _size);

            _rawPointer = rhs._rawPointer;
            _capacity = std::move(rhs._capacity);
            _size = std::move(rhs._size);

            rhs._rawPointer = nullptr;
        }
        return *this;
    }

    template<typename T>
    FS_INLINE T& Vector<T>::operator[](const uint32 index) noexcept
    {
        FS_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[index];
    }

    template<typename T>
    FS_INLINE const T& Vector<T>::operator[](const uint32 index) const noexcept
    {
        FS_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[index];
    }

    template<typename T>
    FS_INLINE void Vector<T>::reserve(uint32 capacity) noexcept
    {
        if (capacity <= _capacity)
        {
            return;
        }

        // 잦은 reserve 시 성능 최적화!!!
        capacity = fs::max(capacity, _capacity * 2);

        T* temp = nullptr;
        if (0 < _size)
        {
            temp = FS_MALLOC(T, _size);

            for (uint32 index = 0; index < _size; ++index)
            {
                FS_PLACEMNT_NEW(&temp[index], T(_rawPointer[index]));
            }
        }

        freeRawPointer(_rawPointer, _size);

        _rawPointer = FS_MALLOC(T, capacity);

        if (0 < _size)
        {
            for (uint32 index = 0; index < _size; ++index)
            {
                FS_PLACEMNT_NEW(&_rawPointer[index], T(temp[index]));
            }

            freeRawPointer(temp, _size);
        }
        
        _capacity = capacity;
    }

    template<typename T>
    FS_INLINE void Vector<T>::resize(const uint32 size) noexcept
    {
        reserve(size);

        if (_size < size)
        {
            for (uint32 index = _size; index < size; ++index)
            {
                FS_PLACEMNT_NEW(&_rawPointer[index], T());
            }
        }
        else if (size < _size)
        {
            for (uint32 index = size; index < _size; ++index)
            {
                _rawPointer[index].~T();
            }
        }

        _size = size;
    }

    template<typename T>
    FS_INLINE void Vector<T>::shrink_to_fit() noexcept
    {
        if (_size < _capacity)
        {
            T* temp = FS_MALLOC(T, _size);
            for (uint32 index = 0; index < _size; ++index)
            {
                FS_PLACEMNT_NEW(&temp[index], T(_rawPointer[index]));
            }

            freeRawPointer(_rawPointer, _size);
            _rawPointer = FS_MALLOC(T, _size);

            for (uint32 index = 0; index < _size; ++index)
            {
                FS_PLACEMNT_NEW(&_rawPointer[index], T(temp[index]));
            }

            freeRawPointer(temp, _size);

            _capacity = _size;
        }
    }

    template<typename T>
    inline void Vector<T>::freeRawPointer(T*& rawPointer, const uint32 size) noexcept
    {
        if (rawPointer == nullptr)
        {
            return;
        }

        for (uint32 index = 0; index < size; ++index)
        {
            rawPointer[index].~T();
        }

        FS_FREE(rawPointer);
    }

    template<typename T>
    FS_INLINE void Vector<T>::clear() noexcept
    {
        while (empty() == false)
        {
            pop_back();
        }
    }

    template<typename T>
    FS_INLINE void Vector<T>::push_back(const T& newEntry) noexcept
    {
        expandCapacityIfNecessary();

        FS_PLACEMNT_NEW(&_rawPointer[_size], T(newEntry));

        ++_size;
    }

    template<typename T>
    FS_INLINE void Vector<T>::push_back(T&& newEntry) noexcept
    {
        expandCapacityIfNecessary();

        FS_PLACEMNT_NEW(&_rawPointer[_size], T(std::move(newEntry)));

        ++_size;
    }

    template<typename T>
    FS_INLINE void Vector<T>::pop_back() noexcept
    {
        if (_size == 0)
        {
            return;
        }
        
        _rawPointer[_size - 1].~T();

        --_size;
    }

    template<typename T>
    FS_INLINE void Vector<T>::insert(const T& newEntry, const uint32 at) noexcept
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
    FS_INLINE void Vector<T>::erase(const uint32 at) noexcept
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
    FS_INLINE void Vector<T>::expandCapacityIfNecessary() noexcept
    {
        if (_size == _capacity)
        {
            reserve((0 == _capacity) ? kBaseCapacity : _capacity * 2);
        }
    }

    template<typename T>
    FS_INLINE T& Vector<T>::front() noexcept
    {
        FS_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[0];
    }

    template<typename T>
    FS_INLINE const T& Vector<T>::front() const noexcept
    {
        FS_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[0];
    }

    template<typename T>
    FS_INLINE T& Vector<T>::back() noexcept
    {
        FS_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[_size - 1];
    }

    template<typename T>
    FS_INLINE const T& Vector<T>::back() const noexcept
    {
        FS_ASSERT("김장원", 0 < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[_size - 1];
    }

    template<typename T>
    FS_INLINE T& Vector<T>::at(const uint32 index) noexcept
    {
        FS_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[std::min(index, _size - 1)];
    }

    template<typename T>
    FS_INLINE const T& Vector<T>::at(const uint32 index) const noexcept
    {
        FS_ASSERT("김장원", index < _size, "범위를 벗어난 접근입니다.");
        return _rawPointer[std::min(index, _size - 1)];
    }

    template<typename T>
    FS_INLINE const T* Vector<T>::data() const noexcept
    {
        return _rawPointer;
    }

    template<typename T>
    FS_INLINE const uint32 Vector<T>::capacity() const noexcept
    {
        return _capacity;
    }

    template<typename T>
    FS_INLINE const uint32 Vector<T>::size() const noexcept
    {
        return _size;
    }

    template<typename T>
    FS_INLINE const bool Vector<T>::empty() const noexcept
    {
        return (_size == 0);
    }
}


#endif // !FS_VECTOR_HPP
