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
    template <typename T>
    class UniqueStringID;

    template <typename T>
    class UniqueString;

    template <typename T>
    class UniqueStringPool;
    
    using UniqueStringA = UniqueString<char>;
    using UniqueStringW = UniqueString<wchar_t>;

    using UniqueStringPoolA = UniqueStringPool<char>;
    using UniqueStringPoolW = UniqueStringPool<wchar_t>;


    template <typename T>
    class UniqueStringID
    {
        friend UniqueString;
        friend UniqueStringPool;

    private:
        static constexpr uint32 kInvalidRawID = kUint32Max;

    public:
                                UniqueStringID();

#if !defined MINT_UNIQUE_STRING_EXPOSE_ID
    private:
#endif
                                UniqueStringID(const uint32 newRawID);

    public:
                                UniqueStringID(const UniqueStringID& rhs) = default;
                                UniqueStringID(UniqueStringID&& rhs) noexcept = default;
                                ~UniqueStringID() = default;

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    public:
#else
    private:
#endif
        UniqueStringID&         operator=(const UniqueStringID& rhs) = default;
        UniqueStringID&         operator=(UniqueStringID && rhs) noexcept = default;
        
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    public:
#else
    private:
#endif
        bool                    operator==(const UniqueStringID& rhs) const noexcept;
        bool                    operator!=(const UniqueStringID& rhs) const noexcept;

    private:
        uint32                  _rawID;
    };


    template <typename T>
    class UniqueString
    {
    public:
        static const UniqueStringID<T>  kInvalidID;

    public:
                                        UniqueString();
    explicit                            UniqueString(const T* const rawString);
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    explicit                            UniqueString(const UniqueStringID<T> id);
#endif
                                        UniqueString(const UniqueString& rhs) = default;
                                        UniqueString(UniqueString&& rhs) noexcept = default;
                                        ~UniqueString() = default;

    public:
        UniqueString&                   operator=(const UniqueString& rhs) = default;
        UniqueString&                   operator=(UniqueString&& rhs) noexcept = default;

    public:
        bool                            operator==(const UniqueString& rhs) const noexcept;
        bool                            operator!=(const UniqueString& rhs) const noexcept;

    public:
        const T*                        c_str() const noexcept;
#if defined MINT_UNIQUE_STRING_EXPOSE_ID
        UniqueStringID<T>               getID() const noexcept;
#endif

    private:
        static UniqueStringPool<T>      _pool;

    private:
        UniqueStringID<T>               _id;

#if defined MINT_DEBUG
    private:
        const T*                        _str;
#endif
    };
    

    template <typename T>
    class UniqueStringPool final
    {
        friend UniqueString;

        static constexpr uint32     kDefaultRawCapacity = 1024;

    private:
                                    UniqueStringPool();
                                    ~UniqueStringPool();

    public:
        UniqueStringID<T>           registerString(const T* const rawString) noexcept;
        bool                        isValid(const UniqueStringID<T> id) const noexcept;
        const T*                    getRawString(const UniqueStringID<T> id) const noexcept;

    public:
        void                        reserve(const uint32 rawCapacity);

    private:
        std::mutex                  _mutex;

        HashMap<const T*, UniqueStringID<T>>    _registrationMap;

    private:
        Vector<uint32>              _offsetArray;
        T*                          _rawMemory;
        uint32                      _rawCapacity;
        uint32                      _totalLength;
        uint32                      _uniqueStringCount;
    };
}


#endif // !MINT_UNIQUE_STRING_H
