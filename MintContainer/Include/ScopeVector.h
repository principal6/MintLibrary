#pragma once


#ifndef _MINT_CONTAINER_SCOPE_VECTOR_H_
#define _MINT_CONTAINER_SCOPE_VECTOR_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    template<typename T, const uint32 Capacity>
    class ScopeVector
    {
    public:
                    ScopeVector();
                    ~ScopeVector();

    public:
        T&          operator[](const uint32 index);
        const T&    operator[](const uint32 index) const;

    public:
        void        resize(const uint32 size);
        void        push_back(const T& entry);
        void        push_back(T&& entry);
        void        pop_back();
        void        clear();
    
    public:
        T&          front();
        const T&    front() const;
        T&          back();
        const T&    back() const;
        T&          at(const uint32 index) noexcept;
        const T&    at(const uint32 index) const noexcept;
        const T*    data() const noexcept { return _array; }

    public:
        constexpr uint32    capacity() const { return Capacity; }
        const uint32        size() const { return _size; }
        const bool          empty() const { return size() == 0; }
        const bool          full() const { return size() == Capacity; }

    private:
        T           _array[Capacity];
        uint32      _size;
    };
}


#endif // !_MINT_CONTAINER_SCOPE_VECTOR_H_
