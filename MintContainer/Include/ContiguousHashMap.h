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

	private:
		HashMap<Key, IndexType> _keyMap;
		Vector<Value> _values;
	};
}


#endif // !_MINT_CONTAINER_CONTIGUOUS_HASH_MAP_H_
