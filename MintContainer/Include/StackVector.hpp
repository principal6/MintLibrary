#pragma once


#ifndef _MINT_CONTAINER_STACK_VECTOR_HPP_
#define _MINT_CONTAINER_STACK_VECTOR_HPP_


#include <MintContainer/Include/StackVector.h>


namespace mint
{
	template<typename T, const uint32 Capacity>
	inline StackVectorStorage<T, Capacity>::StackVectorStorage()
		: _array{}
		, _size{ 0 }
	{
		__noop;
	}
	
	template<typename T, const uint32 Capacity>
	inline StackVectorStorage<T, Capacity>::StackVectorStorage(const std::initializer_list<T>& initializerList)
		: _array{}
		, _size{ 0 }
	{
		for (const T& item : initializerList)
		{
			PushBack(item);
		}
	}

	template<typename T, uint32 Capacity>
	inline StackVectorStorage<T, Capacity>::~StackVectorStorage()
	{
		__noop;
	}

	template<typename T, uint32 Capacity>
	inline void StackVectorStorage<T, Capacity>::Resize(const uint32 size)
	{
		MINT_ASSERT(size <= Capacity, "size 는 Capacity 보다 커질 수 없습니다!", size, Capacity);
		if (size == _size)
		{
			return;
		}

		_size = size;
	}

	template<typename T, uint32 Capacity>
	inline void StackVectorStorage<T, Capacity>::PushBack(const T& entry)
	{
		if (IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 가득 차있습니다!");
			return;
		}

		_array[_size] = entry;
		++_size;
	}

	template<typename T, uint32 Capacity>
	inline void StackVectorStorage<T, Capacity>::PushBack(T&& entry)
	{
		if (IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 가득 차있습니다!");
			return;
		}

		_array[_size] = std::move(entry);
		++_size;
	}

	template<typename T, uint32 Capacity>
	inline void StackVectorStorage<T, Capacity>::PopBack()
	{
		if (IsEmpty())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 비어 있습니다!");
			return;
		}

		--_size;
	}

	template<typename T, uint32 Capacity>
	inline void StackVectorStorage<T, Capacity>::Insert(const T& newEntry, const uint32 at) noexcept
	{
		MINT_ASSERT(at <= _size, "범위를 벗어난 접근입니다. [at: %d / size: %d]", at, _size);
		if (IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 가득 차있습니다!");
			return;
		}

		if constexpr (MemoryRaw::IsMovable<T>() == true)
		{
			MemoryRaw::MoveConstruct<T>(_array[_size], std::move(_array[_size - 1]));

			for (uint32 iter = _size - 1; iter > at; --iter)
			{
				MemoryRaw::MoveAssign<T>(_array[iter], std::move(_array[iter - 1]));
			}
		}
		else // 비효율적이지만 동작은 하도록 한다.
		{
			MemoryRaw::CopyConstruct<T>(_array[_size], _array[_size - 1]);

			for (uint32 iter = _size - 1; iter > at; --iter)
			{
				MemoryRaw::CopyAssign<T>(_array[iter], _array[iter - 1]);
			}
		}
		MemoryRaw::CopyAssign<T>(_array[at], newEntry);
		++_size;
	}

	template<typename T, uint32 Capacity>
	inline void StackVectorStorage<T, Capacity>::Insert(T&& newEntry, const uint32 at) noexcept
	{
		MINT_ASSERT(at <= _size, "범위를 벗어난 접근입니다. [at: %d / size: %d]", at, _size);
		if (IsFull())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 가득 차있습니다!");
			return;
		}

		if constexpr (MemoryRaw::IsMovable<T>() == true)
		{
			MemoryRaw::MoveConstruct<T>(_array[_size], std::move(_array[_size - 1]));

			for (uint32 iter = _size - 1; iter > at; --iter)
			{
				MemoryRaw::MoveAssign<T>(_array[iter], std::move(_array[iter - 1]));
			}

			MemoryRaw::MoveAssign<T>(_array[at], std::move(newEntry));
		}
		else // 비효율적이지만 동작은 하도록 한다.
		{
			MemoryRaw::CopyConstruct<T>(_array[_size], _array[_size - 1]);

			for (uint32 iter = _size - 1; iter > at; --iter)
			{
				MemoryRaw::CopyAssign<T>(_array[iter], _array[iter - 1]);
			}

			MemoryRaw::CopyAssign<T>(_array[at], newEntry);
		}
		++_size;
	}

	template<typename T, uint32 Capacity>
	inline void StackVectorStorage<T, Capacity>::Erase(const uint32 at) noexcept
	{
		MINT_ASSERT(at < _size, "범위를 벗어난 접근입니다. [at: %d / size: %d]", at, _size);
		if (IsEmpty())
		{
			MINT_ASSERT(false, "StackVectorStorage 가 이미 비어 있습니다!");
			return;
		}

		if constexpr (MemoryRaw::IsMovable<T>() == true)
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				MemoryRaw::MoveAssign<T>(_array[iter - 1], std::move(_array[iter]));
			}
		}
		else // 비효율적이지만 동작은 하도록 한다.
		{
			for (uint32 iter = at + 1; iter < _size; ++iter)
			{
				MemoryRaw::CopyAssign<T>(_array[iter - 1], _array[iter]);
			}
		}

		//MemoryRaw::Destroy<T>(_array[_size - 1]);

		--_size;
	}
}


#endif // !_MINT_CONTAINER_STACK_VECTOR_HPP_
