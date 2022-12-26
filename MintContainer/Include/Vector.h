#pragma once


#ifndef _MINT_CONTAINER_VECTOR_H_
#define _MINT_CONTAINER_VECTOR_H_


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
        T*              data() noexcept;
        const T*        data() const noexcept;

    public:
        uint32          capacity() const noexcept;
        uint32          size() const noexcept;
        bool            empty() const noexcept;
    
    public:
        template<typename T>
        class Iterator
        {
        public:
                        Iterator(T* rawPointer) : _rawPointer{ rawPointer } { __noop; }
        
        public:
            bool        operator==(const Iterator& rhs) const noexcept
            {
                return _rawPointer == rhs._rawPointer;
            }

            bool        operator!=(const Iterator& rhs) const noexcept
            {
                return !(*this == rhs);
            }

            Iterator&   operator++() noexcept
            {
                ++_rawPointer;
                return *this;
            }

            T&          operator*() noexcept
            {
                return *_rawPointer;
            }

        private:
            T*          _rawPointer;
        };

        Iterator<T>     begin() noexcept;
        Iterator<T>     end() noexcept;

    private:
        T*              _rawPointer;
        uint32          _capacity;
        uint32          _size;
    };
}


#endif // !_MINT_CONTAINER_VECTOR_H_
