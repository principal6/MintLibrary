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
	inline void StackVectorStorage<T, kCapacity>::Resize(uint32 size)
	{
		MINT_ASSERT(size <= kCapacity, "size(%u) cannot be bigger than kCapacity(%u)!", size, kCapacity);
		if (size == _size)
		{
			return;
		}

		T* const data = Data();
		if (size > _size)
		{
			for (uint32 at = _size; at < size; ++at)
			{
				MemoryRaw::ConstructAt(data[at]);
			}
		}
		else
		{
			MINT_ASSERT(size < _size, "This must be guaranteed by if statements above!");
			for (uint32 at = size; at < _size; ++at)
			{
				MemoryRaw::DestroyAt(data[at]);
			}
		}

		_size = size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::PushBack(const T& entry)
	{
		if (BasicVectorStorage<T>::IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage is already full! PushBack failed.");
			return;
		}

		T* const data = Data();
		MemoryRaw::CopyConstructAt(data[_size], entry);
		++_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::PushBack(T&& entry)
	{
		if (BasicVectorStorage<T>::IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage is already full! PushBack failed.");
			return;
		}

		T* const data = Data();
		MemoryRaw::MoveConstructAt(data[_size], std::move(entry));
		++_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::PopBack() noexcept
	{
		if (BasicVectorStorage<T>::IsEmpty())
		{
			MINT_ASSERT(false, "StackVectorStorage is already empty! PopBack failed.");
			return;
		}

		T* const data = Data();
		MemoryRaw::DestroyAt(data[_size - 1]);
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

		if (BasicVectorStorage<T>::IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage is already full! Insert failed.");
			return false;
		}

		T* const data = Data();
		if constexpr (IsMovable<T>() == true)
		{
			MemoryRaw::MoveConstructAt(data[_size], std::move(data[_size - 1]));
			for (uint32 iter = _size - 1; iter > at; --iter)
			{
				data[iter] = std::move(data[iter - 1]);
			}
		}
		else // Though inefficient, make it work.
		{
			MemoryRaw::CopyConstructAt(data[_size], data[_size - 1]);
			for (uint32 iter = _size; iter > at; --iter)
			{
				data[iter] = data[iter - 1];
			}
		}
		data[at] = newEntry;
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

		if (BasicVectorStorage<T>::IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage is already full! Insert failed.");
			return false;
		}

		T* const data = Data();
		if constexpr (IsMovable<T>() == true)
		{
			MemoryRaw::MoveConstructAt(data[_size], std::move(data[_size - 1]));
			for (uint32 iter = _size - 1; iter > at; --iter)
			{
				data[iter] = std::move(data[iter - 1]);
			}
		}
		else // Though inefficient, make it work.
		{
			MemoryRaw::CopyConstructAt(data[_size], data[_size - 1]);
			for (uint32 iter = _size; iter > at; --iter)
			{
				data[iter] = data[iter - 1];
			}
		}
		data[at] = std::move(newEntry);
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

		if (at == _size - 1)
		{
			PopBack();
			return;
		}
		MINT_ASSERT(at < _size - 1, "This must be guaranteed by if statement above!");

		T* const data = Data();
		if constexpr (IsMovable<T>() == true)
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				data[iter - 1] = std::move(data[iter]);
			}
		}
		else // 비효율적이지만 동작은 하도록 한다.
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				data[iter - 1] = data[iter];
			}
		}
		MemoryRaw::DestroyAt(data[_size - 1]);
		--_size;
	}

	template<typename T, const uint32 kCapacity>
	inline void StackVectorStorage<T, kCapacity>::Clear() noexcept
	{
		T* const data = Data();
		for (uint32 at = 0; at < _size; ++at)
		{
			MemoryRaw::DestroyAt(data[at]);
		}
		_size = 0;
	}
}


#endif // !_MINT_CONTAINER_STACK_VECTOR_HPP_
