#pragma once


#ifndef _MINT_CONTAINER_QUEUE_HPP_
#define _MINT_CONTAINER_QUEUE_HPP_


#include <MintContainer/Include/Queue.h>
#include <MintContainer/Include/MemoryRaw.hpp>


namespace mint
{
	template<typename T>
	inline Queue<T>::Queue()
		: _rawPointer{ nullptr }
		, _size{ 0 }
		, _capacity{ 0 }
		, _headAt{ 0 }
		, _tailAt{ 0 }
	{
		__noop;
	}

	template<typename T>
	inline Queue<T>::~Queue()
	{
		Flush();

		MemoryRaw::DeallocateMemory<T>(_rawPointer);
	}

	template<typename T>
	MINT_INLINE void Queue<T>::Reserve(uint32 capacity) noexcept
	{
		capacity = Max(capacity, static_cast<uint32>(1)); // capacity 가 0 이 되지 않도록 보장!
		if (capacity < _capacity)
		{
			return;
		}

		if (_size == 0)
		{
			MemoryRaw::DeallocateMemory<T>(_rawPointer);
			_rawPointer = MemoryRaw::AllocateMemory<T>(capacity);
			_capacity = capacity;
			// Reset head and tail position!!!
			_headAt = 0;
			_tailAt = _capacity - 1;
			return;
		}

		T* copyPointer = MemoryRaw::AllocateMemory<T>(_size);
		SaveBackup(copyPointer);

		// Reallocate _rawPointer
		MemoryRaw::DeallocateMemory<T>(_rawPointer);
		_rawPointer = MemoryRaw::AllocateMemory<T>(capacity);

		RestoreBackup(copyPointer);
		MemoryRaw::DeallocateMemory<T>(copyPointer);

		// Reset head and tail position!!!
		_headAt = 0;
		_tailAt = _size - 1;

		_capacity = capacity;
	}

	template<typename T>
	MINT_INLINE Queue<T>::ConstIterator Queue<T>::begin() const
	{
		return ConstIterator(*this, _headAt, false);
	}

	template<typename T>
	MINT_INLINE Queue<T>::ConstIterator Queue<T>::end() const
	{
		return ConstIterator(*this, kUint32Max, true);
	}

	template<typename T>
	MINT_INLINE void Queue<T>::SaveBackup(T*& backUpPointer) noexcept
	{
		// Copy data from _rawPointer to copyPointer 
		if (_headAt < _tailAt)
		{
			MemoryRaw::MoveMemory_<T>(backUpPointer, &_rawPointer[_headAt], _size);
			return;
		}

		// Head part
		const uint32 headPartLength = _capacity - _headAt;
		MemoryRaw::MoveMemory_<T>(&backUpPointer[0], &_rawPointer[_headAt], headPartLength);

		// Tail part
		if (headPartLength < _size)
		{
			MemoryRaw::MoveMemory_<T>(&backUpPointer[headPartLength], &_rawPointer[0], _tailAt);
		}
	}

	template<typename T>
	MINT_INLINE void Queue<T>::RestoreBackup(const T* const backUpPointer) noexcept
	{
		MemoryRaw::MoveMemory_<T>(_rawPointer, backUpPointer, _size);
	}

	template<typename T>
	MINT_INLINE void Queue<T>::Push(const T& newEntry) noexcept
	{
		if (_size == _capacity)
		{
			Reserve(_capacity * 2);
		}

		if (_tailAt == _capacity - 1)
		{
			MemoryRaw::CopyConstruct<T>(_rawPointer[0], newEntry);

			_tailAt = 0;
		}
		else
		{
			MemoryRaw::CopyConstruct<T>(_rawPointer[_tailAt + 1], newEntry);

			++_tailAt;
		}

		++_size;
	}

	template<typename T>
	MINT_INLINE void Queue<T>::Push(T&& newEntry) noexcept
	{
		if (_size == _capacity)
		{
			Reserve(_capacity * 2);
		}

		if (_tailAt == _capacity - 1)
		{
			MemoryRaw::MoveConstruct<T>(_rawPointer[0], std::move(newEntry));

			_tailAt = 0;
		}
		else
		{
			MemoryRaw::MoveConstruct<T>(_rawPointer[_tailAt + 1], std::move(newEntry));

			++_tailAt;
		}

		++_size;
	}

	template<typename T>
	MINT_INLINE void Queue<T>::Pop() noexcept
	{
		if (IsEmpty())
		{
			return;
		}

		MemoryRaw::Destroy<T>(_rawPointer[_headAt]);
		++_headAt;

		if (_headAt == _capacity)
		{
			_headAt = 0;
		}

		--_size;
	}

	template<typename T>
	MINT_INLINE void Queue<T>::Flush() noexcept
	{
		while (IsEmpty() == false)
		{
			Pop();
		}
	}

	template<typename T>
	MINT_INLINE T& Queue<T>::Peek() noexcept
	{
		MINT_ASSERT(_size > 0, "_size 가 0 인 Queue 입니다!!!");
		return _rawPointer[_headAt];
	}

	template<typename T>
	MINT_INLINE const T& Queue<T>::Peek() const noexcept
	{
		MINT_ASSERT(_size > 0, "_size 가 0 인 Queue 입니다!!!");
		return _rawPointer[_headAt];
	}

	template<typename T>
	const T& Queue<T>::Get(uint32 index) const noexcept
	{
		uint32 rawIndex = _headAt + index;
		if (rawIndex >= _capacity)
		{
			rawIndex -= _capacity;
		}
		return _rawPointer[rawIndex];
	}
}


#endif // !_MINT_CONTAINER_QUEUE_HPP_
