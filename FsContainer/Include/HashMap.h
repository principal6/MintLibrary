#pragma once


#ifndef FS_HASH_MAP_H
#define FS_HASH_MAP_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Hash.h>
#include <FsContainer/Include/StaticArray.h>
#include <FsContainer/Include/StaticBitArray.h>
#include <FsContainer/Include/Vector.h>


namespace fs
{
    template<typename Key, typename Value>
    class HashMap;


    template<typename Key, typename Value>
    struct KeyValuePair
    {
        const Key*              _key = nullptr;
        Value*                  _value = nullptr;
        FS_INLINE const bool    isValid() const noexcept
        {
            return (_key != nullptr) && (_value != nullptr);
        }
    };

    template<typename Key, typename Value>
    struct KeyValuePairConst
    {
        const Key* _key = nullptr;
        const Value* _value = nullptr;
        FS_INLINE const bool    isValid() const noexcept
        {
            return (_key != nullptr) && (_value != nullptr);
        }
    };


    static constexpr uint32 kHopRange = 16;
    template<typename Key, typename Value>
    struct Bucket
    {
    public:
                                        Bucket();
                                        Bucket(const Bucket& rhs);
                                        ~Bucket() = default;

    public:
        fs::StaticBitArray<kHopRange>   _hopInfo;
        bool                            _isUsed;
        Key                             _key;
        Value                           _value;
    };


    template<typename Key, typename Value>
    class BucketViewer
    {
        template<typename Key, typename Value>
        friend class HashMap;

    private:
                                            BucketViewer(const HashMap<Key, Value>* const hashMap);

    public:
                                            ~BucketViewer() = default;

    public:
        const bool                          isValid() const noexcept;
        void                                next() noexcept;
        KeyValuePairConst<Key, Value>       view() noexcept;

    private:
        const HashMap<Key, Value>* const    _hashMap;
        int32                               _bucketIndex;
    };


    // Hopscotch algorithm
    template<typename Key, typename Value>
    class HashMap final
    {
        template<typename Key, typename Value>
        friend class BucketViewer;

    private:
        static constexpr uint32 kAddRange           = 32;
        static constexpr uint32 kSegmentLength      = 127;

    public:
                                            HashMap();
                                            ~HashMap();
    
    public:
        const bool                          contains(const Key& key) const noexcept;
    
    private:
        const bool                          containsInternal(const uint32 startBucketIndex, const Key& key) const noexcept;

    public:
        void                                insert(const Key& key, const Value& value) noexcept;
        const KeyValuePair<Key, Value>      find(const Key& key) const noexcept;
        const Value&                        at(const Key& key) const noexcept;
        Value&                              at(const Key& key) noexcept;
        void                                erase(const Key& key) noexcept;
        void                                clear() noexcept;
        BucketViewer<Key, Value>            getBucketViewer() const noexcept;

    private:
        static Value&                       getInvalidValue() noexcept;
    
    public:
        const uint32                        size() const noexcept;
        const bool                          empty() const noexcept;

    private:
        void                                resize() noexcept;
    
    private:
        void                                setBucket(const uint32 bucketIndex, const uint32 hopDistance, const Key& key, const Value& value) noexcept;
        const bool                          displace(const uint32 startBucketIndex, uint32& hopDistance) noexcept;
        void                                displaceBucket(const uint32 bucketIndex, const uint32 hopDistanceA, const uint32 hopDistanceB) noexcept;

    private:
        const uint32                        computeSegmentIndex(const uint64 keyHash) const noexcept;
        const uint32                        computeStartBucketIndex(const uint64 keyHash) const noexcept;

    private:
        fs::Vector<Bucket<Key, Value>>      _bucketArray;
        uint32                              _bucketCount;
    };
}


#endif // !FS_HASH_MAP_H
