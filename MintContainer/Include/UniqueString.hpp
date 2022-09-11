#pragma once

#include <MintContainer/Include/UniqueString.h>

#include <MintContainer/Include/StringUtil.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/HashMap.hpp>

namespace mint
{
#pragma region UniqueStringID
    template <typename T>
    inline UniqueStringID<T>::UniqueStringID()
        : _rawID{ UniqueStringID<T>::kInvalidRawID }
    {
        __noop;
    }

    template <typename T>
    inline UniqueStringID<T>::UniqueStringID(const uint32 newRawID)
        : _rawID{ newRawID }
    {
        __noop;
    }

    template <typename T>
    MINT_INLINE bool UniqueStringID<T>::operator==(const UniqueStringID<T>& rhs) const noexcept
    {
        return _rawID == rhs._rawID;
    }

    template <typename T>
    MINT_INLINE bool UniqueStringID<T>::operator!=(const UniqueStringID<T>& rhs) const noexcept
    {
        return _rawID != rhs._rawID;
    }
#pragma endregion


#pragma region UniqueString
    template <typename T>
    inline UniqueString<T>::UniqueString()
        : _id{ kInvalidID }
#if defined MINT_DEBUG
        , _str{}
#endif
    {
        __noop;
    }

    template <typename T>
    inline UniqueString<T>::UniqueString(const T* const rawString)
        : _id{ _pool.registerString(rawString) }
#if defined MINT_DEBUG
        , _str{ _pool.getRawString(_id) }
#endif
    {
        __noop;
    }

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    template <typename T>
    inline UniqueString<T>::UniqueString(const UniqueStringID<T> id)
        : _id{ (_pool.isValid(id)) ? id : UniqueString::kInvalidID }
#if defined MINT_DEBUG
        , _str{ _pool.getRawString(_id) }
#endif
    {
        __noop;
    }
#endif

    template <typename T>
    MINT_INLINE bool UniqueString<T>::operator==(const UniqueString<T>& rhs) const noexcept
    {
        return _id == rhs._id;
    }

    template <typename T>
    MINT_INLINE bool UniqueString<T>::operator!=(const UniqueString<T>& rhs) const noexcept
    {
        return _id != rhs._id;
    }

    template <typename T>
    MINT_INLINE const T* UniqueString<T>::c_str() const noexcept
    {
        return _pool.getRawString(_id);
    }

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    template <typename T>
    MINT_INLINE UniqueStringID<T> UniqueString<T>::getID() const noexcept
    {
        return _id;
    }
#endif
#pragma endregion


#pragma region UniqueStringPool
    template <typename T>
    inline UniqueStringPool<T>::UniqueStringPool()
        : _rawMemory{ nullptr }
        , _rawCapacity{ 0 }
        , _totalLength{ 0 }
        , _uniqueStringCount{ 0 }
    {
        reserve(kDefaultRawCapacity);
    }

    template <typename T>
    inline UniqueStringPool<T>::~UniqueStringPool()
    {
        MINT_DELETE_ARRAY(_rawMemory);
    }

    template <typename T>
    MINT_INLINE UniqueStringID<T> UniqueStringPool<T>::registerString(const T* const rawString) noexcept
    {
        if (rawString == nullptr)
        {
            return UniqueString<T>::kInvalidID;
        }

        auto found = _registrationMap.find(rawString);
        if (found.isValid() == true)
        {
            return *found._value;
        }

        const uint32 lengthNullIncluded = StringUtil::length(rawString) + 1;
        if (_rawCapacity < _totalLength + lengthNullIncluded)
        {
            reserve(_rawCapacity * 2);
        }

        UniqueStringID<T> newID(_uniqueStringCount);
        _offsetArray[newID._rawID] = _totalLength;
        memcpy(&_rawMemory[_offsetArray[newID._rawID]], rawString, lengthNullIncluded - 1);

        _totalLength += lengthNullIncluded;
        ++_uniqueStringCount;
        _registrationMap.insert(rawString, newID);

        return newID;
    }

    template <typename T>
    void UniqueStringPool<T>::reserve(const uint32 rawCapacity)
    {
        std::scoped_lock<std::mutex> scopedLock(_mutex);

        if (_rawCapacity < rawCapacity)
        {
            T* temp = nullptr;
            if (_rawCapacity > 0)
            {
                temp = MINT_NEW_ARRAY(T, _rawCapacity);
                memcpy(temp, _rawMemory, sizeof(T) * _rawCapacity);
            }

            MINT_DELETE_ARRAY(_rawMemory);

            _rawMemory = MINT_NEW_ARRAY(T, rawCapacity);
            if (_rawCapacity > 0)
            {
                memcpy(_rawMemory, temp, sizeof(T) * _rawCapacity);
                MINT_DELETE_ARRAY(temp);
            }

            _rawCapacity = rawCapacity;
            _offsetArray.resize(_rawCapacity);
        }
    }

    template <typename T>
    MINT_INLINE bool UniqueStringPool<T>::isValid(const UniqueStringID<T> id) const noexcept
    {
        return (id._rawID < _uniqueStringCount);
    }

    template <typename T>
    MINT_INLINE const T* UniqueStringPool<T>::getRawString(const UniqueStringID<T> id) const noexcept
    {
        if (isValid(id) == false)
        {
            return nullptr;
        }
        return &_rawMemory[_offsetArray[id._rawID]];;
    }
#pragma endregion
}
