#pragma once


#ifndef _MINT_CONTAINER_STACK_VECTOR_HPP_
#define _MINT_CONTAINER_STACK_VECTOR_HPP_


#include <MintContainer/Include/StackVector.h>


namespace mint
{
	template<typename T, const uint32 kCapacity>
	inline StackVectorStorage<T, kCapacity>::StackVectorStorage()
		: _array{}
		, _size{ 0 }
	{
		__noop;
	}

	template<typename T, const uint32 kCapacity>
	inline StackVectorStorage<T, kCapacity>::StackVectorStorage(const std::initializer_list<T>& initializerList)
		: _array{}
		, _size{ 0 }
	{
		for (const T& item : initializerList)
		{
			PushBack(item);
		}
	}

	template<typename T, const uint32 kCapacity>
	inline StackVectorStorage<T, kCapacity>::~StackVectorStorage()
	{
		__noop;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::Resize(const uint32 size)
	{
		MINT_ASSERT(size <= kCapacity, "size(%u) 는 kCapacity(%u) 보다 커질 수 없습니다!", size, kCapacity);
		if (size == _size)
		{
			return;
		}

		_size = size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::PushBack(const T& entry)
	{
		if (BasicVectorStorage<T>::IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 가득 차있습니다!");
			return;
		}

		_array[_size] = entry;
		++_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::PushBack(T&& entry)
	{
		if (BasicVectorStorage<T>::IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 가득 차있습니다!");
			return;
		}

		_array[_size] = std::move(entry);
		++_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::PopBack() noexcept
	{
		if (BasicVectorStorage<T>::IsEmpty())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 비어 있습니다!");
			return;
		}

		--_size;
	}

	template<typename T, const uint32 kCapacity>
	inline bool StackVectorStorage<T, kCapacity>::Insert(const uint32 at, const T& newEntry)
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

		if constexpr (IsMovable<T>() == true)
		{
			for (uint32 iter = _size; iter > at; --iter)
			{
				_array[iter] = std::move(_array[iter - 1]);
			}
		}
		else // 비효율적이지만 동작은 하도록 한다.
		{
			for (uint32 iter = _size; iter > at; --iter)
			{
				_array[iter] = _array[iter - 1];
			}
		}
		_array[at] = newEntry;
		++_size;
		return true;
	}

	template<typename T, const uint32 kCapacity>
	inline bool StackVectorStorage<T, kCapacity>::Insert(const uint32 at, T&& newEntry)
	{
		if (at > _size)
		{
			return false;
		}

		if (at == _size)
		{
			PushBack(std::move(newEntry));
			return true;
		}
		MINT_ASSERT(_size > 0, "This must be guaranteed by if statement above!");

		if constexpr (IsMovable<T>() == true)
		{
			for (uint32 iter = _size; iter > at; --iter)
			{
				_array[iter] = std::move(_array[iter - 1]);
			}
			_array[at] = std::move(newEntry);
		}
		else // 비효율적이지만 동작은 하도록 한다.
		{
			for (uint32 iter = _size; iter > at; --iter)
			{
				_array[iter] = _array[iter - 1];
			}
			_array[at] = newEntry;
		}
		++_size;
		return true;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::Erase(const uint32 at) noexcept
	{
		if (BasicVectorStorage<T>::IsEmpty())
		{
			return;
		}

		if (at >= _size)
		{
			return;
		}

		if constexpr (IsMovable<T>() == true)
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				_array[iter - 1] = std::move(_array[iter]);
			}
		}
		else // 비효율적이지만 동작은 하도록 한다.
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				_array[iter - 1] = _array[iter];
			}
		}

		--_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::Clear() noexcept
	{
		while (BasicVectorStorage<T>::IsEmpty() == false)
		{
			PopBack();
		}
	}
}


#endif // !_MINT_CONTAINER_STACK_VECTOR_HPP_
