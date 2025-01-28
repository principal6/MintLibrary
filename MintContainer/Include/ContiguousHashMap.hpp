#pragma once


#ifndef _MINT_CONTAINER_CONTIGUOUS_HASH_MAP_HPP_
#define _MINT_CONTAINER_CONTIGUOUS_HASH_MAP_HPP_


#include <MintContainer/Include/ContiguousHashMap.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/HashMap.hpp>


namespace mint
{
	template<typename Key, typename Value>
	inline ContiguousHashMap<Key, Value>::ContiguousHashMap()
	{
		__noop;
	}

	template<typename Key, typename Value>
	inline ContiguousHashMap<Key, Value>::~ContiguousHashMap()
	{
		__noop;
	}

	template<typename Key, typename Value>
	inline bool ContiguousHashMap<Key, Value>::Contains(const Key& key) const noexcept
	{
		return _keyMap.Contains(key);
	}

	template<typename Key, typename Value>
	inline void ContiguousHashMap<Key, Value>::Insert(const Key& key, const Value& value) noexcept
	{
		if (_keyMap.Contains(key) == true)
		{
			return;
		}

		_values.PushBack(value);
		_keyMap.Insert(key, _values.Size() - 1);
	}

	template<typename Key, typename Value>
	inline void ContiguousHashMap<Key, Value>::Insert(const Key& key, Value&& value) noexcept
	{
		if (_keyMap.Contains(key) == true)
		{
			return;
		}

		_values.PushBack(std::move(value));
		_keyMap.Insert(key, _values.Size() - 1);
	}

	template<typename Key, typename Value>
	inline void ContiguousHashMap<Key, Value>::Erase(const Key& key) noexcept
	{
		KeyValuePair<Key, IndexType> keyToIndex = _keyMap.Find(key);
		if (keyToIndex.IsValid() == false)
		{
			return;
		}

		// TODO: add option not to preserve order for better performance
		_values.Erase(*keyToIndex._value);

		_keyMap.Erase(key);
	}

	template<typename Key, typename Value>
	inline void ContiguousHashMap<Key, Value>::Clear() noexcept
	{
		_keyMap.Clear();
		_values.Clear();
	}

	template<typename Key, typename Value>
	inline const Value* ContiguousHashMap<Key, Value>::Find(const Key& key) const noexcept
	{
		KeyValuePair<Key, IndexType> keyToIndex = _keyMap.Find(key);
		if (keyToIndex.IsValid() == false)
		{
			return nullptr;
		}
		return &_values.At(*keyToIndex._value);
	}
	
	template<typename Key, typename Value>
	inline Value* ContiguousHashMap<Key, Value>::Find(const Key& key) noexcept
	{
		KeyValuePair<Key, IndexType> keyToIndex = _keyMap.Find(key);
		if (keyToIndex.IsValid() == false)
		{
			return nullptr;
		}
		return &_values.At(*keyToIndex._value);
	}
	
	template<typename Key, typename Value>
	inline const Value& ContiguousHashMap<Key, Value>::At(const Key& key) const noexcept
	{
		return _values.At(_keyMap.At(key));
	}

	template<typename Key, typename Value>
	inline Value& ContiguousHashMap<Key, Value>::At(const Key& key) noexcept
	{
		return _values.At(_keyMap.At(key));
	}

	template<typename Key, typename Value>
	inline uint32 ContiguousHashMap<Key, Value>::Size() const noexcept
	{
		return _values.Size();
	}

	template<typename Key, typename Value>
	inline bool ContiguousHashMap<Key, Value>::IsEmpty() const noexcept
	{
		return _keyMap.IsEmpty();
	}

	template<typename Key, typename Value>
	inline const Vector<Value>& ContiguousHashMap<Key, Value>::GetValues() const noexcept
	{
		return _values;
	}
}


#endif // !_MINT_CONTAINER_CONTIGUOUS_HASH_MAP_HPP_

