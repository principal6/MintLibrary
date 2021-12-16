#pragma once


#ifndef MINT_CONTAINER_QUEUE_H
#define MINT_CONTAINER_QUEUE_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    template <typename T>
    class Queue
    {
    public:
                                    Queue();
                                    ~Queue();

    public:
        void                        reserve(const uint32 capacity) noexcept;

    public:
        MINT_INLINE const uint32    size() const noexcept { return _size; }
        MINT_INLINE const uint32    capacity() const noexcept { return _capacity; }
        MINT_INLINE const bool      empty() const noexcept { return (_size == 0); }

    public:
        void                        push(const T& newEntry) noexcept;
        void                        push(T&& newEntry) noexcept;
        void                        pop() noexcept;
        void                        flush() noexcept;

    public:
        T&                          peek() noexcept;
        const T&                    peek() const noexcept;

    private:
        T*                          _rawPointer;
        uint32                      _size;
        uint32                      _capacity;
        uint32                      _headAt;
        uint32                      _tailAt;
    };
}


#endif // !MINT_CONTAINER_QUEUE_H
