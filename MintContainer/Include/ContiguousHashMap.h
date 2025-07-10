#pragma once


#ifndef _MINT_CONTAINER_CONTIGUOUS_HASH_MAP_H_
#define _MINT_CONTAINER_CONTIGUOUS_HASH_MAP_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>


namespace mint
{
	template<typename Key, typename Value>
	class ContiguousHashMap
	{
		using IndexType = uint32;

	public:
		ContiguousHashMap();
		~ContiguousHashMap();

	public:
		bool Contains(const Key& key) const noexcept;

	public:
		void Insert(const Key& key, const Value& value) noexcept;
		void Insert(const Key& key, Value&& value) noexcept;

		void Erase(const Key& key) noexcept;
		void Clear() noexcept;

	public:
		const Value* Find(const Key& key) const noexcept;
		Value* Find(const Key& key) noexcept;
		const Value& At(const Key& key) const noexcept;
		Value& At(const Key& key) noexcept;

	public:
		uint32 Size() const noexcept;
		bool IsEmpty() const noexcept;
		const Vector<Value>& GetValues() const noexcept;

	public:
		class Iterator
		{
		public:
			Iterator(HashMap<Key, IndexType>::Iterator&& keyMapIterator, Vector<Value>& values) : _keyMapIterator{ std::move(keyMapIterator) }, _values{ values } { __noop; }
			~Iterator() = default;
		
		public:
			bool operator==(const Iterator& rhs) const noexcept
			{
				return _keyMapIterator == rhs._keyMapIterator;
			}
			bool operator!=(const Iterator& rhs) const noexcept
			{
				return !(*this == rhs);
			}
			Iterator& operator++() noexcept
			{
				++_keyMapIterator;
				return *this;
			}
			Value& operator*() const noexcept
			{
				return GetValue();
			}
			Key& GetKey() const noexcept
			{
				return _keyMapIterator.GetKey();
			}
			Value& GetValue() const noexcept
			{
				return _values[_keyMapIterator.GetValue()];
			}
		private:
			HashMap<Key, IndexType>::Iterator _keyMapIterator;
			Vector<Value>& _values;
		};
		
		Iterator begin() noexcept
		{
			return Iterator(_keyMap.begin(), _values);
		}
		Iterator end() noexcept
		{
			return Iterator(_keyMap.end(), _values);
		}

		class ConstIterator
		{
		public:
			ConstIterator(HashMap<Key, IndexType>::ConstIterator&& keyMapIterator, const Vector<Value>& values) : _keyMapIterator{ std::move(keyMapIterator) }, _values{ values } { __noop; }
			~ConstIterator() = default;
			bool operator==(const ConstIterator& rhs) const noexcept
			{
				return _keyMapIterator == rhs._keyMapIterator;
			}
			bool operator!=(const ConstIterator& rhs) const noexcept
			{
				return !(*this == rhs);
			}
			ConstIterator& operator++() noexcept
			{
				++_keyMapIterator;
				return *this;
			}
			const Value& operator*() const noexcept
			{
				return GetValue();
			}
			const Key& GetKey() const noexcept
			{
				return _keyMapIterator.GetKey();
			}
			const Value& GetValue() const noexcept
			{
				return _values[_keyMapIterator.GetValue()];
			}
		private:
			HashMap<Key, IndexType>::ConstIterator _keyMapIterator;
			const Vector<Value>& _values;
		};

		ConstIterator begin() const noexcept
		{
			return ConstIterator(_keyMap.begin(), _values);
		}

		ConstIterator end() const noexcept
		{
			return ConstIterator(_keyMap.end(), _values);
		}

	private:
		HashMap<Key, IndexType> _keyMap;
		Vector<Value> _values;
	};
}


#endif // !_MINT_CONTAINER_CONTIGUOUS_HASH_MAP_H_
