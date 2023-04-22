#pragma once


#ifndef _MINT_CONTAINER_HASH_MAP_H_
#define _MINT_CONTAINER_HASH_MAP_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Hash.h>
#include <MintContainer/Include/Array.h>
#include <MintContainer/Include/BitArray.h>
#include <MintContainer/Include/Vector.h>


namespace mint
{
	template<typename Key, typename Value>
	class HashMap;


	template<typename Key, typename Value>
	struct KeyValuePair
	{
		const Key* _key = nullptr;
		Value* _value = nullptr;
		MINT_INLINE bool IsValid() const noexcept
		{
			return (_key != nullptr) && (_value != nullptr);
		}
	};

	template<typename Key, typename Value>
	struct KeyValuePairConst
	{
		const Key* _key = nullptr;
		const Value* _value = nullptr;
		MINT_INLINE bool IsValid() const noexcept
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

		template <typename B = Bucket>
		Bucket(std::enable_if_t<std::is_copy_constructible<Value>::value, const B&> rhs);

		template <typename B = Bucket>
		Bucket(std::enable_if_t<std::is_move_constructible<Value>::value, B&&> rhs);

		~Bucket() = default;

	public:
		BitArray<kHopRange> _hopInfo;
		bool _isUsed;
		Key _key;
		Value _value;
	};


	// Hopscotch algorithm
	// Key must be default_constructible and must have bool operator==(const Key&) defined
	template<typename Key, typename Value>
	class HashMap final
	{
		template<typename Key, typename Value>
		friend class BucketViewer;

	public:
		HashMap();
		~HashMap();

	public:
		bool Contains(const Key& key) const noexcept;

	public:
		template<typename V = Value>
		std::enable_if_t<std::is_copy_constructible<V>::value == true || std::is_default_constructible<V>::value, void> Insert(const Key& key, const V& value) noexcept;

		template<typename V = Value>
		std::enable_if_t<std::is_copy_constructible<V>::value == false, void> Insert(const Key& key, V&& value) noexcept;
		
		void Erase(const Key& key) noexcept;
		void Clear() noexcept;

	public:
		KeyValuePair<Key, Value> Find(const Key& key) const noexcept;
		const Value& At(const Key& key) const noexcept;
		Value& At(const Key& key) noexcept;

	public:
		uint32 Size() const noexcept;
		bool IsEmpty() const noexcept;

	public:
		class Iterator
		{
		public:
			Iterator(HashMap<Key, Value>& hashMap, const uint32 bucketIndex) : _hashMap{ hashMap }, _bucketIndex{ bucketIndex } { __noop; }

		public:
			bool operator==(const Iterator& rhs) const noexcept
			{
				return &_hashMap == &rhs._hashMap && _bucketIndex == rhs._bucketIndex;
			}
			bool operator!=(const Iterator& rhs) const noexcept
			{
				return !(*this == rhs);
			}
			Iterator& operator++() noexcept
			{
				_bucketIndex = _hashMap.GetNextValidBucketIndex(_bucketIndex);
				return *this;
			}
			Value& operator*() noexcept
			{
				Bucket<Key, Value>& bucket = _hashMap._bucketArray[_bucketIndex];
				return bucket._value;
			}

		private:
			HashMap<Key, Value>& _hashMap;
			uint32 _bucketIndex;
		};

		Iterator begin() noexcept;
		Iterator end() noexcept;

	private:
		bool ContainsInternal(const uint32 startBucketIndex, const Key& key) const noexcept;

	private:
		bool ExistsEmptySlotInAddRange(const uint32 startBucketIndex, uint32& hopDistance) const noexcept;

	private:
		static Value& GetInvalidValue() noexcept;

	private:
		void Resize() noexcept;

	private:
		void SetBucket(const uint32 bucketIndex, const uint32 hopDistance, const Key& key, const Value& value) noexcept;
		void SetBucket(const uint32 bucketIndex, const uint32 hopDistance, const Key& key, Value&& value) noexcept;
		bool Displace(const uint32 startBucketIndex, uint32& hopDistance) noexcept;
		void DisplaceBucket(const uint32 bucketIndex, const uint32 hopDistanceA, const uint32 hopDistanceB) noexcept;

	private:
		uint32 ComputeSegmentIndex(const uint64 keyHash) const noexcept;
		uint32 ComputeStartBucketIndex(const uint64 keyHash) const noexcept;
		uint32 GetNextValidBucketIndex(const uint32 currentBucketIndex) const;

	private:
		static constexpr uint32 kAddRange = 32;
		static constexpr uint32 kSegmentLength = 127;

	private:
		Vector<Bucket<Key, Value>> _bucketArray;
		uint32 _bucketCount;
	};
}


#endif // !_MINT_CONTAINER_HASH_MAP_H_
