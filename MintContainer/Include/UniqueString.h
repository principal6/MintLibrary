#pragma once


#ifndef MINT_UNIQUE_STRING_H
#define MINT_UNIQUE_STRING_H


#include <mutex>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>


namespace mint
{
    template <typename T>
    class UniqueStringID;

    template <typename T>
    class UniqueString;

    template <typename T>
    class UniqueStringPool;

    template<typename T>
    class StringView;
    
    using UniqueStringA = UniqueString<char>;
    using UniqueStringW = UniqueString<wchar_t>;

    using UniqueStringPoolA = UniqueStringPool<char>;
    using UniqueStringPoolW = UniqueStringPool<wchar_t>;


    template <typename T>
    class UniqueStringID
    {
        friend StringView;
        friend UniqueString;
        friend UniqueStringPool;

    private:
        static constexpr uint32 kInvalidRawID = kUint32Max;

    public:
                                UniqueStringID();

    private:
                                UniqueStringID(const uint32 newRawID);

    public:
                                UniqueStringID(const UniqueStringID& rhs) = default;
                                UniqueStringID(UniqueStringID&& rhs) noexcept = default;
                                ~UniqueStringID() = default;

    private:
        UniqueStringID&         operator=(const UniqueStringID& rhs) = default;
        UniqueStringID&         operator=(UniqueStringID && rhs) noexcept = default;
        
    private:
        bool                    operator==(const UniqueStringID& rhs) const noexcept;
        bool                    operator!=(const UniqueStringID& rhs) const noexcept;

    private:
        uint32                  _rawID;
    };


    template <typename T>
    class UniqueString
    {
        friend StringView;

    public:
        static const UniqueStringID<T>  kInvalidID;

    private:
        static UniqueStringPool<T>      _pool;

    public:
                                        UniqueString();
    explicit                            UniqueString(const T* const rawString);
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
        uint32                          length() const noexcept;

    private:
        UniqueStringID<T>               _id;
        const T*                        _str;
        uint32                          _length;
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
