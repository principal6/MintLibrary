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
    class UniqueStringAId;
    class UniqueStringA;
    class UniqueStringPoolA;


    class UniqueStringAId
    {
        friend UniqueStringA;
        friend UniqueStringPoolA;

    private:
        static constexpr uint32     kInvalidRawId = kUint32Max;
    
    public:
                                    UniqueStringAId();

#if !defined MINT_UNIQUE_STRING_EXPOSE_ID
    private:
#endif
                                    UniqueStringAId(const uint32 newRawId);

    public:
                                    UniqueStringAId(const UniqueStringAId& rhs) = default;
                                    UniqueStringAId(UniqueStringAId&& rhs) noexcept = default;
                                    ~UniqueStringAId() = default;

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    public:
#else
    private:
#endif
        UniqueStringAId&            operator=(const UniqueStringAId& rhs) = default;
        UniqueStringAId&            operator=(UniqueStringAId && rhs) noexcept = default;
        
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    public:
#else
    private:
#endif
        const bool                  operator==(const UniqueStringAId& rhs) const noexcept;
        const bool                  operator!=(const UniqueStringAId& rhs) const noexcept;

    private:
        uint32                      _rawId;
    };


    class UniqueStringA
    {
    public:
        static const UniqueStringAId                kInvalidId;

    public:
                                                    UniqueStringA();
    explicit                                        UniqueStringA(const char* const rawString);
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    explicit                                        UniqueStringA(const UniqueStringAId id);
#endif
                                                    UniqueStringA(const UniqueStringA& rhs) = default;
                                                    UniqueStringA(UniqueStringA&& rhs) noexcept = default;
                                                    ~UniqueStringA() = default;

    public:
        UniqueStringA&                              operator=(const UniqueStringA& rhs) = default;
        UniqueStringA&                              operator=(UniqueStringA&& rhs) noexcept = default;

    public:
        const bool                                  operator==(const UniqueStringA& rhs) const noexcept;
        const bool                                  operator!=(const UniqueStringA& rhs) const noexcept;

    public:
        const char*                                 c_str() const noexcept;
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
        const UniqueStringAId                       getId() const noexcept;
#endif

    private:
        static UniqueStringPoolA                    _pool;

    private:
        UniqueStringAId                             _id;

#if defined MINT_DEBUG
    private:
        const char*                                 _str;
#endif
    };
    

    class UniqueStringPoolA final
    {
        friend UniqueStringA;

        static constexpr uint32                         kDefaultRawCapacity = 1024;

    private:
                                                        UniqueStringPoolA();
                                                        ~UniqueStringPoolA();

    public:
        const UniqueStringAId                           registerString(const char* const rawString) noexcept;
        const bool                                      isValid(const UniqueStringAId id) const noexcept;
        const char*                                     getRawString(const UniqueStringAId id) const noexcept;

    public:
        void                                            reserve(const uint32 rawCapacity);

    private:
        std::mutex                                      _mutex;
        mint::HashMap<uint64, UniqueStringAId>            _registrationMap;

    private:
        mint::Vector<uint32>                              _offsetArray;
        char*                                           _rawMemory;
        uint32                                          _rawCapacity;
        uint32                                          _totalLength;
        uint32                                          _uniqueStringCount;
    };
}


#endif // !MINT_UNIQUE_STRING_H
