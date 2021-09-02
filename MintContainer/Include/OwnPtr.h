#pragma once


#ifndef MINT_CONTAINER_OWN_PTR
#define MINT_CONTAINER_OWN_PTR


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    // TODO: OwnPtr SharedPtr SharedPtrViewer
    template<typename T>
    class OwnPtr
    {
    public:
                    OwnPtr() : _rawPointer{ nullptr } { __noop; }
                    OwnPtr(T* const rawPointer) : _rawPointer{ rawPointer } { __noop; }
                    OwnPtr(const OwnPtr& rhs) = delete;
                    OwnPtr(OwnPtr&& rhs) : _rawPointer{ rhs._rawPointer } { rhs._rawPointer = nullptr; }
                    ~OwnPtr() { release(); }

    public:
        OwnPtr&     operator=(const OwnPtr& rhs)
        {
            if (this != &rhs)
            {
                release();

                assign(MINT_NEW(T, *rhs._rawPointer));
            }
            return *this;
        }

        OwnPtr&     operator=(OwnPtr&& rhs) noexcept
        {
            if (this != &rhs)
            {
                _rawPointer = rhs._rawPointer;
                rhs._rawPointer = nullptr;
            }
            return *this;
        }

    public:
        T&          operator*() noexcept
        {
            return *_rawPointer;
        }

        const T&    operator*() const noexcept
        {
            return *_rawPointer;
        }

        T*          operator->() noexcept
        {
            return _rawPointer;
        }

        const T*    operator->() const noexcept
        {
            return _rawPointer;
        }

    public:
        void        assign(T*&& rawPointer) noexcept
        {
            release();

            _rawPointer = rawPointer;

            rawPointer = nullptr;
        }

        void        release() noexcept
        {
            MINT_DELETE(_rawPointer);
        }

    private:
        T*          _rawPointer;
    };
}


#endif // !MINT_CONTAINER_OWN_PTR
