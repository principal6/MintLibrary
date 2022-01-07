#pragma once


#ifndef MINT_UNIQUE_STRING_H
#define MINT_UNIQUE_STRING_H


#include <mutex>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>


//#define MINT_UNIQUE_STRING_EXPOSE_ID


namespace mint
{
    class UniqueStringAID;
    class UniqueStringA;
    class UniqueStringPoolA;


    class UniqueStringAID
    {
        friend UniqueStringA;
        friend UniqueStringPoolA;

    private:
        static constexpr uint32 kInvalidRawID = kUint32Max;

    public:
                                UniqueStringAID();

#if !defined MINT_UNIQUE_STRING_EXPOSE_ID
    private:
#endif
                                UniqueStringAID(const uint32 newRawID);

    public:
                                UniqueStringAID(const UniqueStringAID& rhs) = default;
                                UniqueStringAID(UniqueStringAID&& rhs) noexcept = default;
                                ~UniqueStringAID() = default;

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    public:
#else
    private:
#endif
        UniqueStringAID&        operator=(const UniqueStringAID& rhs) = default;
        UniqueStringAID&        operator=(UniqueStringAID && rhs) noexcept = default;
        
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    public:
#else
    private:
#endif
        const bool              operator==(const UniqueStringAID& rhs) const noexcept;
        const bool              operator!=(const UniqueStringAID& rhs) const noexcept;

    private:
        uint32                  _rawID;
    };


    class UniqueStringA
    {
    public:
        static const UniqueStringAID    kInvalidID;

    public:
                                        UniqueStringA();
    explicit                            UniqueStringA(const char* const rawString);
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    explicit                            UniqueStringA(const UniqueStringAID id);
#endif
                                        UniqueStringA(const UniqueStringA& rhs) = default;
                                        UniqueStringA(UniqueStringA&& rhs) noexcept = default;
                                        ~UniqueStringA() = default;

    public:
        UniqueStringA&                  operator=(const UniqueStringA& rhs) = default;
        UniqueStringA&                  operator=(UniqueStringA&& rhs) noexcept = default;

    public:
        const bool                      operator==(const UniqueStringA& rhs) const noexcept;
        const bool                      operator!=(const UniqueStringA& rhs) const noexcept;

    public:
        const char*                     c_str() const noexcept;
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
        const UniqueStringAID           getID() const noexcept;
#endif

    private:
        static UniqueStringPoolA        _pool;

    private:
        UniqueStringAID                 _id;

#if defined MINT_DEBUG
    private:
        const char*                     _str;
#endif
    };
    

    class UniqueStringPoolA final
    {
        friend UniqueStringA;

        static constexpr uint32             kDefaultRawCapacity = 1024;

    private:
                                            UniqueStringPoolA();
                                            ~UniqueStringPoolA();

    public:
        const UniqueStringAID               registerString(const char* const rawString) noexcept;
        const bool                          isValid(const UniqueStringAID id) const noexcept;
        const char*                         getRawString(const UniqueStringAID id) const noexcept;

    public:
        void                                reserve(const uint32 rawCapacity);

    private:
        std::mutex                          _mutex;
        HashMap<uint64, UniqueStringAID>    _registrationMap;

    private:
        Vector<uint32>                      _offsetArray;
        char*                               _rawMemory;
        uint32                              _rawCapacity;
        uint32                              _totalLength;
        uint32                              _uniqueStringCount;
    };
}


#endif // !MINT_UNIQUE_STRING_H
