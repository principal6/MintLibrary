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

	template<typename T, uint32 Capacity>
	inline StackVectorStorage<T, Capacity>::~StackVectorStorage()
	{
		__noop;
	}

	template<typename T, uint32 Capacity>
	inline T& StackVectorStorage<T, Capacity>::operator[](const uint32 index)
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다. [index: %d / size: %d]", index, _size);
		return _array[index];
	}

	template<typename T, uint32 Capacity>
	inline const T& StackVectorStorage<T, Capacity>::operator[](const uint32 index) const
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다. [index: %d / size: %d]", index, _size);
		return _array[index];
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

		_array[_size] = entry;
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
	inline void StackVectorStorage<T, Capacity>::Clear()
	{
		while (IsEmpty() == false)
		{
			PopBack();
		}
	}

	template<typename T, uint32 Capacity>
	inline T& StackVectorStorage<T, Capacity>::Front()
	{
		if (_size == 0)
		{
			MINT_NEVER;
		}
		return _array[0];
	}

	template<typename T, uint32 Capacity>
	inline const T& StackVectorStorage<T, Capacity>::Front() const
	{
		if (_size == 0)
		{
			MINT_NEVER;
		}
		return _array[0];
	}

	template<typename T, uint32 Capacity>
	inline T& StackVectorStorage<T, Capacity>::Back()
	{
		if (_size == 0)
		{
			MINT_NEVER;
		}
		return _array[_size - 1];
	}

	template<typename T, uint32 Capacity>
	inline const T& StackVectorStorage<T, Capacity>::Back() const
	{
		if (_size == 0)
		{
			MINT_NEVER;
		}
		return _array[_size - 1];
	}

	template<typename T, uint32 Capacity>
	MINT_INLINE T& StackVectorStorage<T, Capacity>::At(const uint32 index) noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다.");
		return _array[Min(index, _size - 1)];
	}

	template<typename T, uint32 Capacity>
	MINT_INLINE const T& StackVectorStorage<T, Capacity>::At(const uint32 index) const noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다.");
		return _array[Min(index, _size - 1)];
	}
}


#endif // !_MINT_CONTAINER_STACK_VECTOR_HPP_
