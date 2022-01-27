#pragma once


#ifndef MINT_CONTAINER_QUEUE_HPP
#define MINT_CONTAINER_QUEUE_HPP


#include <MintCommon/Include/Logger.h>

#include <MintContainer/Include/Queue.h>
#include <MintContainer/Include/MemoryRaw.hpp>


namespace mint
{
    template<typename T>
    inline Queue<T>::Queue()
        : _rawPointer{ nullptr }
        , _size{ 0 }
        , _capacity{ 0 }
        , _headAt{ 0 }
        , _tailAt{ 0 }
    {
        __noop;
    }

    template<typename T>
    inline Queue<T>::~Queue()
    {
        flush();

        MemoryRaw::deallocateMemory<T>(_rawPointer);
    }

    template<typename T>
    MINT_INLINE void Queue<T>::reserve(uint32 capacity) noexcept
    {
        capacity = mint::max(capacity, static_cast<uint32>(1)); // capacity 가 0 이 되지 않도록 보장!
        if (capacity < _capacity)
        {
            return;
        }

        if (_size == 0)
        {
            MemoryRaw::deallocateMemory<T>(_rawPointer);
            _rawPointer = MemoryRaw::allocateMemory<T>(capacity);
            _capacity = capacity;
            return;
        }

        T* copyPointer = MemoryRaw::allocateMemory<T>(_size);
        saveBackup(copyPointer);

        // Reallocate _rawPointer
        MemoryRaw::deallocateMemory<T>(_rawPointer);
        _rawPointer = MemoryRaw::allocateMemory<T>(capacity);

        restoreBackup(copyPointer);
        MemoryRaw::deallocateMemory<T>(copyPointer);

        // Reset head and tail position!!!
        _headAt = 0;
        _tailAt = _size - 1;

        _capacity = capacity;
    }

    template<typename T>
    MINT_INLINE void Queue<T>::saveBackup(T*& backUpPointer) noexcept
    {
        // Copy data from _rawPointer to copyPointer 
        if (_headAt < _tailAt)
        {
            MemoryRaw::moveMemory<T>(backUpPointer, &_rawPointer[_headAt], _size);
            return;
        }
        
        // Head part
        const uint32 headPartLength = _capacity - _headAt;
        MemoryRaw::moveMemory<T>(&backUpPointer[0], &_rawPointer[_headAt], headPartLength);

        // Tail part
        if (headPartLength < _size)
        {
            MemoryRaw::moveMemory<T>(&backUpPointer[headPartLength], &_rawPointer[0], _tailAt + 1);
        }
    }

    template<typename T>
    MINT_INLINE void Queue<T>::restoreBackup(const T* const backUpPointer) noexcept
    {
        MemoryRaw::moveMemory<T>(_rawPointer, backUpPointer, _size);
    }

    template<typename T>
    MINT_INLINE void Queue<T>::push(const T& newEntry) noexcept
    {
        if (_size == _capacity)
        {
            reserve(_capacity * 2);
        }

        if (_tailAt == _capacity - 1)
        {
            MemoryRaw::copyConstruct<T>(_rawPointer[0], newEntry);

            _tailAt = 0;
        }
        else
        {
            MemoryRaw::copyConstruct<T>(_rawPointer[_tailAt + 1], newEntry);

            ++_tailAt;
        }

        ++_size;
    }

    template<typename T>
    MINT_INLINE void Queue<T>::push(T&& newEntry) noexcept
    {
        if (_size == _capacity)
        {
            reserve(_capacity * 2);
        }

        if (_tailAt == _capacity - 1)
        {
            MemoryRaw::moveConstruct<T>(_rawPointer[0], std::move(newEntry));

            _tailAt = 0;
        }
        else
        {
            MemoryRaw::moveConstruct<T>(_rawPointer[_tailAt + 1], std::move(newEntry));

            ++_tailAt;
        }

        ++_size;
    }

    template<typename T>
    MINT_INLINE void Queue<T>::pop() noexcept
    {
        if (empty() == true)
        {
            return;
        }

        MemoryRaw::destroy<T>(_rawPointer[_headAt]);
        ++_headAt;

        if (_headAt == _capacity)
        {
            _headAt = 0;
        }

        --_size;
    }

    template<typename T>
    MINT_INLINE void Queue<T>::flush() noexcept
    {
        while (empty() == false)
        {
            pop();
        }
    }

    template<typename T>
    MINT_INLINE T& Queue<T>::peek() noexcept
    {
        MINT_ASSERT("김장원", _size > 0, "_size 가 0 인 Queue 입니다!!!");
        return _rawPointer[_headAt];
    }

    template<typename T>
    MINT_INLINE const T& Queue<T>::peek() const noexcept
    {
        MINT_ASSERT("김장원", _size > 0, "_size 가 0 인 Queue 입니다!!!");
        return _rawPointer[_headAt];
    }
}


#endif // !MINT_CONTAINER_QUEUE_HPP
