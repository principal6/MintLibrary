#pragma once


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
    inline Vector<T>::Vector(const uint32 capacity)
        : Vector()
    {
        reserve(capacity);
    }

    template<typename T>
    inline Vector<T>::Vector(const std::initializer_list<T>& initializerList)
    {
        const size_t count = initializerList.size();
        resize(count);

        const T* const first = initializerList.begin();
        for (size_t index = 0; index < count; ++index)
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
        : _rawPointer{ std::move(rhs._rawPointer) }
        , _capacity{ std::move(rhs._capacity) }
        , _size{ std::move(rhs._size) }
    {
        __noop;
    }

    template<typename T>
    inline Vector<T>::~Vector()
    {
        FS_DELETE_ARRAY(_rawPointer);
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
            FS_DELETE_ARRAY(_rawPointer);

            _rawPointer = std::move(rhs._rawPointer);
            _capacity = std::move(rhs._capacity);
            _size = std::move(rhs._size);
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
    FS_INLINE void Vector<T>::reserve(const uint32 capacity) noexcept
    {
        if (capacity <= _capacity)
        {
            return;
        }

        if (0 < _size)
        {
            T* temp = FS_NEW_ARRAY(T, _size);
            for (uint32 index = 0; index < _size; ++index)
            {
                temp[index] = _rawPointer[index];
            }

            FS_DELETE_ARRAY(_rawPointer);
            _rawPointer = FS_NEW_ARRAY(T, capacity);

            for (uint32 index = 0; index < _size; ++index)
            {
                _rawPointer[index] = temp[index];
            }

            FS_DELETE_ARRAY(temp);
        }
        else
        {
            FS_DELETE_ARRAY(_rawPointer);
            _rawPointer = FS_NEW_ARRAY(T, capacity);
        }

        _capacity = capacity;
    }

    template<typename T>
    FS_INLINE void Vector<T>::resize(const uint32 size) noexcept
    {
        reserve(size);

        _size = size;
    }

    template<typename T>
    FS_INLINE void Vector<T>::shrink_to_fit() noexcept
    {
        if (_size < _capacity)
        {
            T* temp = FS_NEW_ARRAY(T, _size);
            for (uint32 index = 0; index < _size; ++index)
            {
                temp[index] = _rawPointer[index];
            }

            FS_DELETE_ARRAY(_rawPointer);
            _rawPointer = FS_NEW_ARRAY(T, _size);

            for (uint32 index = 0; index < _size; ++index)
            {
                _rawPointer[index] = temp[index];
            }

            FS_DELETE_ARRAY(temp);

            _capacity = _size;
        }
    }

    template<typename T>
    FS_INLINE void Vector<T>::clear() noexcept
    {
        _size = 0;
    }

    template<typename T>
    FS_INLINE void Vector<T>::push_back(const T& newEntry) noexcept
    {
        expandCapacityIfNecessary();

        _rawPointer[_size] = newEntry;
        ++_size;
    }

    template<typename T>
    FS_INLINE void Vector<T>::push_back(T&& newEntry) noexcept
    {
        expandCapacityIfNecessary();

        _rawPointer[_size] = std::move(newEntry);
        ++_size;
    }

    template<typename T>
    FS_INLINE void Vector<T>::pop_back() noexcept
    {
        if (_size == 0)
        {
            return;
        }
        
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
    FS_INLINE const uint32 Vector<T>::capacity() const noexcept
    {
        return _capacity;
    }

    template<typename T>
    FS_INLINE const uint32 Vector<T>::size() const noexcept
    {
        return _size;
    }

}