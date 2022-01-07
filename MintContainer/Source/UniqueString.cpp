#include <MintContainer/Include/UniqueString.h>
#include <MintContainer/Include/UniqueString.hpp>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/HashMap.hpp>


namespace mint
{
    const UniqueStringAID UniqueStringA::kInvalidID;
    UniqueStringPoolA UniqueStringA::_pool;
    UniqueStringA::UniqueStringA()
        : _id{ UniqueStringA::kInvalidID }
#if defined MINT_DEBUG
        , _str{}
#endif
    {
        __noop;
    }

    UniqueStringA::UniqueStringA(const char* const rawString)
        : _id{ _pool.registerString(rawString) }
#if defined MINT_DEBUG
        , _str{ _pool.getRawString(_id) }
#endif
    {
        __noop;
    }

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    UniqueStringA::UniqueStringA(const UniqueStringAID id)
        : _id{ (_pool.isValid(id)) ? id : UniqueStringA::kInvalidID }
#if defined MINT_DEBUG
        , _str{ _pool.getRawString(_id) }
#endif
    {
        __noop;
    }
#endif


    UniqueStringPoolA::UniqueStringPoolA()
        : _rawMemory{ nullptr }
        , _rawCapacity{ 0 }
        , _totalLength{ 0 }
        , _uniqueStringCount{ 0 }
    {
        reserve(kDefaultRawCapacity);
    }

    UniqueStringPoolA::~UniqueStringPoolA()
    {
        MINT_DELETE_ARRAY(_rawMemory);
    }

    const UniqueStringAID UniqueStringPoolA::registerString(const char* const rawString) noexcept
    {
        if (rawString == nullptr)
        {
            return UniqueStringA::kInvalidID;
        }

        const uint64 hash = computeHash(rawString);
        {
            auto found = _registrationMap.find(hash);
            if (found.isValid() == true)
            {
                return *found._value;
            }
        }

        const uint32 lengthNullIncluded = StringUtil::length(rawString) + 1;
        if (_rawCapacity < _totalLength + lengthNullIncluded)
        {
            reserve(_rawCapacity * 2);
        }

        UniqueStringAID newID(_uniqueStringCount);
        _offsetArray[newID._rawID] = _totalLength;
        memcpy(&_rawMemory[_offsetArray[newID._rawID]], rawString, lengthNullIncluded - 1);

        _totalLength += lengthNullIncluded;
        ++_uniqueStringCount;
        _registrationMap.insert(hash, newID);

        return newID;
    }

    void UniqueStringPoolA::reserve(const uint32 rawCapacity)
    {
        std::scoped_lock<std::mutex> scopedLock(_mutex);

        if (_rawCapacity < rawCapacity)
        {
            char* temp = nullptr;
            if (_rawCapacity > 0)
            {
                temp = MINT_NEW_ARRAY(char, _rawCapacity);
                memcpy(temp, _rawMemory, sizeof(char) * _rawCapacity);
            }
            
            MINT_DELETE_ARRAY(_rawMemory);

            _rawMemory = MINT_NEW_ARRAY(char, rawCapacity);
            if (_rawCapacity > 0)
            {
                memcpy(_rawMemory, temp, sizeof(char) * _rawCapacity);
                MINT_DELETE_ARRAY(temp);
            }

            _rawCapacity = rawCapacity;
            _offsetArray.resize(_rawCapacity);
        }
    }
}
