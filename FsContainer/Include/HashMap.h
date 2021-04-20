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
    struct KeyValuePair
    {
        const Key*      _key = nullptr;
        const Value*    _value = nullptr;
    };

    // Hopscotch algorithm
    template<typename Key, typename Value>
    class HashMap final
    {
    private:
        static constexpr uint32 kHopRange           = 16;
        static constexpr uint32 kAddRange           = 32;
        static constexpr uint32 kSegmentLength      = 127;

        struct Bucket
        {
            fs::StaticBitArray<kHopRange>   _hopInfo;
            bool                            _isUsed;
            Key                             _key;
            Value                           _value;
        };

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
        void                                erase(const Key& key) noexcept;
        void                                clear() noexcept;

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
        fs::Vector<Bucket>                  _bucketArray;
        uint32                              _bucketCount;
    };
}


#endif // !FS_HASH_MAP_H
