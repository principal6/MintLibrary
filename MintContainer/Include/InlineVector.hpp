#pragma once


#ifndef _MINT_CONTAINER_INLINE_VECTOR_HPP_
#define _MINT_CONTAINER_INLINE_VECTOR_HPP_


#include <MintContainer/Include/InlineVector.h>
#include <MintContainer/Include/MemoryRaw.hpp>


namespace mint
{
	template<typename T, const uint32 kCapacity>
	inline InlineVectorStorage<T, kCapacity>::InlineVectorStorage()
		: _capacity{ kCapacity }
		, _size{ 0 }
		, _ptr{ reinterpret_cast<T*>(__array) }
		, __array{}
	{
		__noop;
	}

	template<typename T, const uint32 kCapacity>
	inline InlineVectorStorage<T, kCapacity>::InlineVectorStorage(const std::initializer_list<T>& initializerList)
		: _capacity{ kCapacity }
		, _size{ 0 }
		, _ptr{ reinterpret_cast<T*>(__array) }
		, __array{}
	{
		Reserve(static_cast<uint32>(initializerList.size()));

		for (const T& item : initializerList)
		{
			PushBack(item);
		}
	}

	template<typename T, const uint32 kCapacity>
	inline InlineVectorStorage<T, kCapacity>::~InlineVectorStorage()
	{
		Clear();
	}

	template<typename T, const uint32 kCapacity>
	inline void InlineVectorStorage<T, kCapacity>::Reserve(const uint32 newCapacity)
	{
		if (newCapacity <= _capacity)
		{
			return;
		}

		const uint32 sizeCache = _size;
		T* newPtr = MINT_MALLOC(T, newCapacity);
		if constexpr (IsMovable<T>() == true)
		{
			for (uint32 at = 0; at < sizeCache; ++at)
			{
				MemoryRaw::MoveConstructAt(newPtr[at], std::move(_ptr[at]));
			}
			Clear();
		}
		else // though inefficient, make it work.
		{
			for (uint32 at = 0; at < sizeCache; ++at)
			{
				MemoryRaw::CopyConstructAt(newPtr[at], _ptr[at]);
			}
			Clear();
		}
		_ptr = newPtr;
		_size = sizeCache;
		_capacity = newCapacity;
		MINT_ASSERT(IsUsingHeap() == true, "This must be guaranteed when Reserve() is processed.");
	}

	template<typename T, const uint32 kCapacity>
	inline void InlineVectorStorage<T, kCapacity>::Resize(const uint32 newSize) requires (IsDefaultConstructible<T>() == true)
	{
		if (newSize == _size)
		{
			return;
		}
		
		if (newSize > _size)
		{
			Reserve(newSize);

			for (uint32 at = _size; at < newSize; ++at)
			{
				MemoryRaw::ConstructAt(_ptr[at]);
			}
		}
		else
		{
			MINT_ASSERT(newSize < _size, "This must be guaranteed by if statement above!");
			for (uint32 at = newSize; at < _size; ++at)
			{
				MemoryRaw::DestroyAt(_ptr[at]);
			}
		}

		_size = newSize;
	}

	template<typename T, const uint32 kCapacity>
	inline void InlineVectorStorage<T, kCapacity>::PushBack(const T& entry)
	{
		if (IsFull())
		{
			Reserve(Capacity() * 2);
		}

		MemoryRaw::CopyConstructAt(_ptr[_size], entry);
		++_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void InlineVectorStorage<T, kCapacity>::PushBack(T&& entry)
	{
		if (IsFull())
		{
			Reserve(Capacity() * 2);
		}

		MemoryRaw::MoveConstructAt(_ptr[_size], std::move(entry));
		++_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void InlineVectorStorage<T, kCapacity>::PopBack()
	{
		if (IsEmpty())
		{
			return;
		}

		MemoryRaw::DestroyAt(_ptr[_size - 1]);
		--_size;
	}

	template<typename T, const uint32 kCapacity>
	inline bool InlineVectorStorage<T, kCapacity>::Insert(const uint32 at, const T& newEntry)
	{
		if (at > _size)
		{
			return false;
		}

		if (at == _size)
		{
			PushBack(newEntry);
			return true;
		}
		MINT_ASSERT(_size > 0, "This must be guaranteed by if statement above!");

		if (IsFull())
		{
			Reserve(Capacity() * 2);
		}
		if constexpr (IsMovable<T>() == true)
		{
			MemoryRaw::MoveConstructAt(_ptr[_size], std::move(_ptr[_size - 1]));
			for (uint32 iter = _size - 1; iter > at; --iter)
			{
				_ptr[iter] = std::move(_ptr[iter - 1]);
			}
		}
		else // Though inefficient, make it work.
		{
			MemoryRaw::CopyConstructAt(_ptr[_size], _ptr[_size - 1]);
			for (uint32 iter = _size; iter > at; --iter)
			{
				_ptr[iter] = _ptr[iter - 1];
			}
		}
		_ptr[at] = newEntry;
		++_size;
		return true;
	}

	template<typename T, const uint32 kCapacity>
	inline bool InlineVectorStorage<T, kCapacity>::Insert(const uint32 at, T&& newEntry)
	{
		if (at > _size)
		{
			return false;
		}

		if (at >= _size)
		{
			PushBack(std::move(newEntry));
			return true;
		}
		MINT_ASSERT(_size > 0, "This must be guaranteed by if statement above!");

		if (IsFull())
		{
			Reserve(Capacity() * 2);
		}
		MemoryRaw::MoveConstructAt(_ptr[_size], std::move(_ptr[_size - 1]));
		for (uint32 iter = _size - 1; iter > at; --iter)
		{
			_ptr[iter] = std::move(_ptr[iter - 1]);
		}
		_ptr[at] = std::move(newEntry);
		++_size;
		return true;
	}

	template<typename T, const uint32 kCapacity>
	inline void InlineVectorStorage<T, kCapacity>::Erase(const uint32 at) noexcept
	{
		if (IsEmpty())
		{
			return;
		}

		if (at >= _size)
		{
			return;
		}

		if (at == _size - 1)
		{
			PopBack();
			return;
		}
		MINT_ASSERT(at < _size - 1, "This must be guaranteed by if statement above!");
		
		if constexpr (IsMovable<T>() == true)
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				_ptr[iter - 1] = std::move(_ptr[iter]);
			}
		}
		else // Though inefficient, make it work.
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				_ptr[iter - 1] = _ptr[iter];
			}
		}
		MemoryRaw::DestroyAt(_ptr[_size - 1]);
		--_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void InlineVectorStorage<T, kCapacity>::Clear() noexcept
	{
		for (uint32 at = 0; at < _size; ++at)
		{
			MemoryRaw::DestroyAt(_ptr[at]);
		}

		if (IsUsingHeap())
		{
			MINT_FREE(_ptr);
			_ptr = reinterpret_cast<T*>(__array);
		}

		_size = 0;
		_capacity = kCapacity;
		MINT_ASSERT(IsUsingHeap() == false, "This must be guaranteed after Clear() is processed.");
		MINT_ASSERT(IsEmpty() == true, "This must be guaranteed after Clear() is processed.");
	}
}


#endif // !_MINT_CONTAINER_INLINE_VECTOR_HPP_
