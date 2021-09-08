#pragma once


#ifndef MINT_VECTOR_H
#define MINT_VECTOR_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace std
{
    template <class _Elem>
    class initializer_list;
}


namespace mint
{
    template <typename T>
    class Vector final
    {
        static constexpr uint32 kBaseCapacity = 8;

    public:
                        Vector();
                        Vector(const uint32 size);
                        Vector(const std::initializer_list<T>& initializerList);
                        Vector(const Vector& rhs) noexcept;
                        Vector(Vector&& rhs) noexcept;
                        ~Vector();

    public:
        Vector&         operator=(const Vector& rhs) noexcept;
        Vector&         operator=(Vector&& rhs) noexcept;

    public:
        T&              operator[](const uint32 index) noexcept;
        const T&        operator[](const uint32 index) const noexcept;

    public:
        void            reserve(const uint32 capacity) noexcept;
        void            resize(const uint32 size) noexcept; // default-constructible 의 경우에만 호출 가능하다!
        void            shrink_to_fit() noexcept;

    public:
        void            clear() noexcept;
        void            push_back(const T& newEntry) noexcept;
        void            push_back(T&& newEntry) noexcept;
        void            pop_back() noexcept;
        void            insert(const T& newEntry, const uint32 at) noexcept;
        void            insert(T&& newEntry, const uint32 at) noexcept;
        void            erase(const uint32 at) noexcept;

    private:
        void            expandCapacityIfNecessary() noexcept;

    public:
        T&              front() noexcept;
        const T&        front() const noexcept;
        T&              back() noexcept;
        const T&        back() const noexcept;
        T&              at(const uint32 index) noexcept;
        const T&        at(const uint32 index) const noexcept;
        const T*        data() const noexcept;

    public:
        const uint32    capacity() const noexcept;
        const uint32    size() const noexcept;
        const bool      empty() const noexcept;

    private:
        T*              _rawPointer;
        uint32          _capacity;
        uint32          _size;
    };
}


#endif // !MINT_VECTOR_H
