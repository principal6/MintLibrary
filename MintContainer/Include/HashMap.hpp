#pragma once


#ifndef _MINT_CONTAINER_HASH_MAP_HPP_
#define _MINT_CONTAINER_HASH_MAP_HPP_


#include <MintContainer/Include/HashMap.h>

#include <MintContainer/Include/Hash.hpp>
#include <MintContainer/Include/BitArray.hpp>
#include <MintContainer/Include/Vector.hpp>


namespace mint
{
	template<typename Key, typename Value>
	inline Bucket<Key, Value>::Bucket()
		: _isUsed{ false }
		, _key{}
		, _value{}
	{
		__noop;
	}

	template<typename Key, typename Value>
	template<typename B>
	inline Bucket<Key, Value>::Bucket(std::enable_if_t<std::is_copy_constructible<Value>::value, const B&> rhs)
		: _hopInfo{ rhs._hopInfo }
		, _isUsed{ rhs._isUsed }
		, _key{ rhs._key }
		, _value{ rhs._value }
	{
		__noop;
	}

	template<typename Key, typename Value>
	template<typename B>
	inline Bucket<Key, Value>::Bucket(std::enable_if_t<std::is_move_constructible<Value>::value, B&&> rhs)
		: _hopInfo{ rhs._hopInfo }
		, _isUsed{ rhs._isUsed }
		, _key{ rhs._key }
		, _value{ std::move(rhs._value) }
	{
		__noop;
	}


	template<typename Key, typename Value>
	inline HashMap<Key, Value>::HashMap()
		: _bucketArray{ kSegmentLength }
		, _bucketCount{ 0 }
	{
		__noop;
	}

	template<typename Key, typename Value>
	inline HashMap<Key, Value>::~HashMap()
	{
		__noop;
	}

	template<typename Key, typename Value>
	inline bool HashMap<Key, Value>::Contains(const Key& key) const noexcept
	{
		const uint64 keyHash = Hasher<Key>()(key);
		const uint32 startBucketIndex = ComputeStartBucketIndex(keyHash);
		return ContainsInternal(startBucketIndex, key);
	}

	template<typename Key, typename Value>
	inline bool HashMap<Key, Value>::ContainsInternal(const uint32 startBucketIndex, const Key& key) const noexcept
	{
		auto& startBucket = _bucketArray[startBucketIndex];
		for (uint32 hopAt = 0; hopAt < kHopRange; ++hopAt)
		{
			if (startBucket._hopInfo.Get(hopAt) == true && _bucketArray[startBucketIndex + hopAt]._key == key)
			{
				return true;
			}
		}
		return false;
	}

	template<typename Key, typename Value>
	void HashMap<Key, Value>::Insert(const Key& key, const Value& value) noexcept
	{
		const uint64 keyHash = Hasher<Key>()(key);
		const uint32 startBucketIndex = ComputeStartBucketIndex(keyHash);

		if (ContainsInternal(startBucketIndex, key) == true)
		{
			return;
		}

		auto& startBucket = _bucketArray[startBucketIndex];
		if (startBucket._isUsed == false)
		{
			SetBucket(startBucketIndex, 0, key, value);
			return;
		}

		uint32 hopDistance;
		if (ExistsEmptySlotInAddRange(startBucketIndex, hopDistance) == true)
		{
			// Check if it is closest

			if (hopDistance < kHopRange)
			{
				SetBucket(startBucketIndex, hopDistance, key, value);
				return;
			}

			do
			{
				if (Displace(startBucketIndex, hopDistance) == false)
				{
					break;
				}
			} while (kHopRange <= hopDistance);

			if (hopDistance < kHopRange)
			{
				SetBucket(startBucketIndex, hopDistance, key, value);
				return;
			}
		}

		Resize();
		Insert(key, value);
	}

	template<typename Key, typename Value>
	inline void HashMap<Key, Value>::Insert(const Key& key, Value&& value) noexcept
	{
		const uint64 keyHash = Hasher<Key>()(key);
		const uint32 startBucketIndex = ComputeStartBucketIndex(keyHash);

		if (ContainsInternal(startBucketIndex, key) == true)
		{
			return;
		}

		auto& startBucket = _bucketArray[startBucketIndex];
		if (startBucket._isUsed == false)
		{
			SetBucket(startBucketIndex, 0, key, std::move(value));
			return;
		}

		uint32 hopDistance;
		if (ExistsEmptySlotInAddRange(startBucketIndex, hopDistance) == true)
		{
			// Check if it is closest

			if (hopDistance < kHopRange)
			{
				SetBucket(startBucketIndex, hopDistance, key, std::move(value));
				return;
			}

			do
			{
				if (Displace(startBucketIndex, hopDistance) == false)
				{
					break;
				}
			} while (kHopRange <= hopDistance);

			if (hopDistance < kHopRange)
			{
				SetBucket(startBucketIndex, hopDistance, key, std::move(value));
				return;
			}
		}

		Resize();
		Insert(key, std::move(value));
	}

	template<typename Key, typename Value>
	inline bool HashMap<Key, Value>::ExistsEmptySlotInAddRange(const uint32 startBucketIndex, uint32& hopDistance) const noexcept
	{
		hopDistance = 0;
		for (; hopDistance < kAddRange; ++hopDistance)
		{
			if (_bucketArray.Size() <= startBucketIndex + hopDistance)
			{
				break;
			}

			if (_bucketArray[startBucketIndex + hopDistance]._isUsed == false)
			{
				return true;
			}
		}
		return false;
	}

	template<typename Key, typename Value>
	inline KeyValuePair<Key, Value> HashMap<Key, Value>::Find(const Key& key) const noexcept
	{
		const uint64 keyHash = Hasher<Key>()(key);
		const uint32 startBucketIndex = ComputeStartBucketIndex(keyHash);
		const Bucket<Key, Value>& startBucket = _bucketArray[startBucketIndex];
		KeyValuePair<Key, Value> findResult;
		for (uint32 hopAt = 0; hopAt < kHopRange; ++hopAt)
		{
			if (startBucket._hopInfo.Get(hopAt) == true && _bucketArray[startBucketIndex + hopAt]._key == key)
			{
				findResult._key = &_bucketArray[startBucketIndex + hopAt]._key;
				findResult._value = const_cast<Value*>(&_bucketArray[startBucketIndex + hopAt]._value);
				break;
			}
		}
		return findResult;
	}

	template<typename Key, typename Value>
	inline const Value& HashMap<Key, Value>::At(const Key& key) const noexcept
	{
		const Value* const value = Find(key)._value;
		return (value == nullptr) ? GetInvalidValue() : *value;
	}

	template<typename Key, typename Value>
	inline Value& HashMap<Key, Value>::At(const Key& key) noexcept
	{
		Value* const value = Find(key)._value;
		return (value == nullptr) ? GetInvalidValue() : *value;
	}

	template<typename Key, typename Value>
	inline void HashMap<Key, Value>::Erase(const Key& key) noexcept
	{
		const uint64 keyHash = Hasher<Key>()(key);
		const uint32 startBucketIndex = ComputeStartBucketIndex(keyHash);
		auto& startBucket = _bucketArray[startBucketIndex];
		int32 hopDistance = -1;
		for (uint32 hopAt = 0; hopAt < kHopRange; ++hopAt)
		{
			if (startBucket._hopInfo.Get(hopAt) == true && _bucketArray[startBucketIndex + hopAt]._key == key)
			{
				hopDistance = static_cast<int32>(hopAt);
				break;
			}
		}
		if (hopDistance >= 0)
		{
			startBucket._hopInfo.Set(hopDistance, false);

			_bucketArray[startBucketIndex + hopDistance]._isUsed = false;

			--_bucketCount;
		}
	}

	template<typename Key, typename Value>
	inline void HashMap<Key, Value>::Clear() noexcept
	{
		_bucketArray.Clear();
		_bucketArray.Resize(kSegmentLength);

		_bucketCount = 0;
	}

	template<typename Key, typename Value>
	inline Value& HashMap<Key, Value>::GetInvalidValue() noexcept
	{
		static Value invalidValue{};
		return invalidValue;
	}

	template<typename Key, typename Value>
	inline uint32 HashMap<Key, Value>::Size() const noexcept
	{
		return _bucketCount;
	}

	template<typename Key, typename Value>
	inline bool HashMap<Key, Value>::IsEmpty() const noexcept
	{
		return _bucketCount == 0;
	}

	template<typename Key, typename Value>
	inline void HashMap<Key, Value>::Resize() noexcept
	{
		const float load = static_cast<float>(_bucketCount) / static_cast<float>(_bucketArray.Size());
		MINT_LOG("HashMap resizes with load [%f, %d/%d]", load, _bucketCount, _bucketArray.Size());

		Vector<Bucket<Key, Value>> oldBucketArray = _bucketArray;

		_bucketCount = 0;

		_bucketArray.Clear();
		_bucketArray.Resize(oldBucketArray.Size() * 2);

		const uint32 oldBucketCount = oldBucketArray.Size();
		for (uint32 oldBucketIndex = 0; oldBucketIndex < oldBucketCount; ++oldBucketIndex)
		{
			if (oldBucketArray[oldBucketIndex]._isUsed == true)
			{
				if constexpr (std::is_copy_constructible<Value>::value == true)
				{
					Insert(oldBucketArray[oldBucketIndex]._key, oldBucketArray[oldBucketIndex]._value);
				}
				else
				{
					Insert(oldBucketArray[oldBucketIndex]._key, std::move(oldBucketArray[oldBucketIndex]._value));
				}
			}
		}
	}

	template<typename Key, typename Value>
	inline void HashMap<Key, Value>::SetBucket(const uint32 bucketIndex, const uint32 hopDistance, const Key& key, const Value& value) noexcept
	{
		_bucketArray[bucketIndex]._hopInfo.Set(hopDistance, true);

		_bucketArray[bucketIndex + hopDistance]._isUsed = true;
		_bucketArray[bucketIndex + hopDistance]._key = key;

		if constexpr (std::is_copy_assignable<Value>::value == true)
		{
			_bucketArray[bucketIndex + hopDistance]._value = value;
		}
		else
		{
			_bucketArray[bucketIndex + hopDistance]._value.~Value();
			MINT_PLACEMNT_NEW(&_bucketArray[bucketIndex + hopDistance]._value, Value(value));
		}

		++_bucketCount;
	}

	template<typename Key, typename Value>
	inline void HashMap<Key, Value>::SetBucket(const uint32 bucketIndex, const uint32 hopDistance, const Key& key, Value&& value) noexcept
	{
		_bucketArray[bucketIndex]._hopInfo.Set(hopDistance, true);

		_bucketArray[bucketIndex + hopDistance]._isUsed = true;
		_bucketArray[bucketIndex + hopDistance]._key = key;

		if constexpr (std::is_move_assignable<Value>::value == true)
		{
			_bucketArray[bucketIndex + hopDistance]._value = std::move(value);
		}
		else
		{
			_bucketArray[bucketIndex + hopDistance]._value.~Value();
			MINT_PLACEMNT_NEW(&_bucketArray[bucketIndex + hopDistance]._value, Value(std::move(value)));
		}

		++_bucketCount;
	}

	template<typename Key, typename Value>
	inline bool HashMap<Key, Value>::Displace(const uint32 startBucketIndex, uint32& hopDistance) noexcept
	{
		const uint32 bucketH_1Index = startBucketIndex + hopDistance - (kHopRange - 1);
		auto& bucketH_1 = _bucketArray[bucketH_1Index];
		for (uint32 hopAt = 0; hopAt < kHopRange; ++hopAt)
		{
			if (bucketH_1._hopInfo.Get(hopAt) == true)
			{
				DisplaceBucket(bucketH_1Index, hopAt, 3);
				hopDistance = bucketH_1Index + hopAt - startBucketIndex;
				return true;
			}
		}
		return false;
	}

	template<typename Key, typename Value>
	inline void HashMap<Key, Value>::DisplaceBucket(const uint32 bucketIndex, const uint32 hopDistanceA, const uint32 hopDistanceB) noexcept
	{
		auto& baseBucket = _bucketArray[bucketIndex];
		auto& bucketA = _bucketArray[bucketIndex + hopDistanceA];
		auto& bucketB = _bucketArray[bucketIndex + hopDistanceB];
		MINT_ASSERT(hopDistanceA < kHopRange, "HopDistance 는 반드시 HopRange 안에 있어야 합니다!!!");
		MINT_ASSERT(hopDistanceB < kHopRange, "HopDistance 는 반드시 HopRange 안에 있어야 합니다!!!");
		MINT_ASSERT(bucketB._isUsed == false, "BucketB 는 비어 있어야만 합니다!!!");

		baseBucket._hopInfo.Set(hopDistanceA, false);
		baseBucket._hopInfo.Set(hopDistanceB, true);

		bucketA._isUsed = false;

		bucketB._isUsed = true;
		bucketB._key = std::move(bucketA._key);

		if constexpr (std::is_move_assignable<Value>::value == true)
		{
			bucketB._value = std::move(bucketA._value);
		}
		else
		{
			bucketB._value.~Value();
			MINT_PLACEMNT_NEW(&bucketB._value, Value(std::move(bucketA._value)));
		}
	}

	template<typename Key, typename Value>
	MINT_INLINE uint32 HashMap<Key, Value>::ComputeSegmentIndex(const uint64 keyHash) const noexcept
	{
		return keyHash % (_bucketArray.Capacity() / kSegmentLength);
	}

	template<typename Key, typename Value>
	MINT_INLINE uint32 HashMap<Key, Value>::ComputeStartBucketIndex(const uint64 keyHash) const noexcept
	{
		return (kSegmentLength * ComputeSegmentIndex(keyHash)) + keyHash % kSegmentLength;
	}
	
	template<typename Key, typename Value>
	MINT_INLINE uint32 HashMap<Key, Value>::GetNextValidBucketIndex(const uint32 currentBucketIndex) const
	{
		const uint32 bucketArraySize = _bucketArray.Size();
		for (uint32 bucketIndex = currentBucketIndex + 1; bucketIndex < bucketArraySize; ++bucketIndex)
		{
			if (_bucketArray[bucketIndex]._isUsed == true)
			{
				return bucketIndex;
			}
		}
		return bucketArraySize;
	}

	template<typename Key, typename Value>
	MINT_INLINE HashMap<Key, Value>::Iterator HashMap<Key, Value>::begin() noexcept
	{
		const int32 bucketArraySize = static_cast<int32>(_bucketArray.Size());
		int32 firstBucketIndex = bucketArraySize;
		for (int32 bucketIndex = 0; bucketIndex < bucketArraySize; ++bucketIndex)
		{
			if (_bucketArray[bucketIndex]._isUsed == true)
			{
				firstBucketIndex = bucketIndex;
				break;
			}
		}
		return Iterator(*this, firstBucketIndex);
	}

	template<typename Key, typename Value>
	MINT_INLINE HashMap<Key, Value>::Iterator HashMap<Key, Value>::end() noexcept
	{
		const int32 bucketArraySize = static_cast<int32>(_bucketArray.Size());
		return Iterator(*this, bucketArraySize);
	}

	template<typename Key, typename Value>
	MINT_INLINE HashMap<Key, Value>::ConstIterator HashMap<Key, Value>::begin() const noexcept
	{
		const int32 bucketArraySize = static_cast<int32>(_bucketArray.Size());
		int32 firstBucketIndex = bucketArraySize;
		for (int32 bucketIndex = 0; bucketIndex < bucketArraySize; ++bucketIndex)
		{
			if (_bucketArray[bucketIndex]._isUsed == true)
			{
				firstBucketIndex = bucketIndex;
				break;
			}
		}
		return ConstIterator(*this, firstBucketIndex);
	}

	template<typename Key, typename Value>
	MINT_INLINE HashMap<Key, Value>::ConstIterator HashMap<Key, Value>::end() const noexcept
	{
		const int32 bucketArraySize = static_cast<int32>(_bucketArray.Size());
		return ConstIterator(*this, bucketArraySize);
	}
}


#endif // !_MINT_CONTAINER_HASH_MAP_HPP_
