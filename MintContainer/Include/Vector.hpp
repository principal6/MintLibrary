#pragma once


#ifndef _MINT_CONTAINER_VECTOR_HPP_
#define _MINT_CONTAINER_VECTOR_HPP_


#include <initializer_list>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/MemoryRaw.hpp>


namespace mint
{
	template <typename T>
	inline Vector<T>::Vector()
		: _rawPointer{ nullptr }
		, _capacity{ 0 }
		, _size{ 0 }
	{
		__noop;
	}

	template<typename T>
	inline Vector<T>::Vector(const uint32 size)
		: Vector()
	{
		Resize(size);
	}

	template<typename T>
	inline Vector<T>::Vector(const std::initializer_list<T>& initializerList)
		: Vector()
	{
		const uint32 count = static_cast<uint32>(initializerList.size());
		Reserve(count);

		const T* const rawPointer = initializerList.begin();
		for (uint32 index = 0; index < count; ++index)
		{
			PushBack(rawPointer[index]);
		}
	}

	template<typename T>
	inline Vector<T>::Vector(const Vector& rhs) noexcept
		: Vector()
	{
		Reserve(rhs._size);

		for (uint32 index = 0; index < rhs._size; ++index)
		{
			PushBack(rhs._rawPointer[index]);
		}
	}

	template<typename T>
	inline Vector<T>::Vector(Vector&& rhs) noexcept
		: _rawPointer{ rhs._rawPointer }
		, _capacity{ rhs._capacity }
		, _size{ rhs._size }
	{
		rhs._rawPointer = nullptr;
		rhs._capacity = 0;
		rhs._size = 0;
	}

	template<typename T>
	inline Vector<T>::~Vector()
	{
		Clear();

		MemoryRaw::DeallocateMemory<T>(_rawPointer);
	}

	template<typename T>
	MINT_INLINE Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) noexcept
	{
		if (this != &rhs)
		{
			Clear();

			Reserve(rhs._size);

			for (uint32 index = 0; index < rhs._size; ++index)
			{
				PushBack(rhs._rawPointer[index]);
			}
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept
	{
		if (this != &rhs)
		{
			Clear();

			MemoryRaw::DeallocateMemory<T>(_rawPointer);

			_rawPointer = rhs._rawPointer;
			_capacity = rhs._capacity;
			_size = rhs._size;

			rhs._rawPointer = nullptr;
			rhs._capacity = 0;
			rhs._size = 0;
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE T& Vector<T>::operator[](const uint32 index) noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다. [index: %d / size: %d]", index, _size);
		return _rawPointer[index];
	}

	template<typename T>
	MINT_INLINE const T& Vector<T>::operator[](const uint32 index) const noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다. [index: %d / size: %d]", index, _size);
		return _rawPointer[index];
	}

	template<typename T>
	MINT_INLINE void Vector<T>::Reserve(const uint32 capacity) noexcept
	{
		if (capacity <= _capacity)
		{
			return;
		}

		// 잦은 Reserve 시 성능 최적화!!!
		_capacity = Max(capacity, _capacity * 2);

		if (_size == 0)
		{
			MemoryRaw::DeallocateMemory<T>(_rawPointer);
			_rawPointer = MemoryRaw::AllocateMemory<T>(_capacity);
		}
		else
		{
			T* temp = MemoryRaw::AllocateMemory<T>(_size);
			MemoryRaw::MoveMemory_<T>(temp, _rawPointer, _size);

			MemoryRaw::DeallocateMemory<T>(_rawPointer);
			_rawPointer = MemoryRaw::AllocateMemory<T>(_capacity);

			MemoryRaw::MoveMemory_<T>(_rawPointer, temp, _size);
			MemoryRaw::DeallocateMemory<T>(temp);
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::Resize(const uint32 size) noexcept
	{
		if (_size < size)
		{
			Reserve(size);
			for (uint32 index = _size; index < size; ++index)
			{
				MemoryRaw::Construct(_rawPointer[index]);
			}
			_size = size;
		}
		else if (size < _size)
		{
			const uint32 oldSize = _size;
			for (uint32 index = size; index < oldSize; ++index)
			{
				PopBack();
			}
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::ShrinkToFit() noexcept
	{
		if (_capacity <= _size)
		{
			return;
		}

		T* temp = MemoryRaw::AllocateMemory<T>(_size);
		MemoryRaw::MoveMemory_<T>(temp, _rawPointer, _size);

		MemoryRaw::DeallocateMemory<T>(_rawPointer);
		_rawPointer = MemoryRaw::AllocateMemory<T>(_size);

		MemoryRaw::MoveMemory_<T>(_rawPointer, temp, _size);
		MemoryRaw::DeallocateMemory<T>(temp);

		_capacity = _size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::Clear() noexcept
	{
		while (IsEmpty() == false)
		{
			PopBack();
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::PushBack(const T& newEntry) noexcept
	{
		ExpandCapacityIfNecessary();

		MemoryRaw::CopyConstruct<T>(_rawPointer[_size], newEntry);

		++_size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::PushBack(T&& newEntry) noexcept
	{
		ExpandCapacityIfNecessary();

		MemoryRaw::MoveConstruct<T>(_rawPointer[_size], std::move(newEntry));

		++_size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::PopBack() noexcept
	{
		if (_size == 0)
		{
			return;
		}

		// valid 한 범위 내 [0, _size - 1] element 는
		// 모두 ctor 의 호출이 보장되었으므로
		// 반드시 Destroy() 를 호출해야 한다.
		MemoryRaw::Destroy<T>(_rawPointer[_size - 1]);

		--_size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::Insert(const T& newEntry, const uint32 at) noexcept
	{
		if (_size <= at)
		{
			PushBack(newEntry);
		}
		else
		{
			ExpandCapacityIfNecessary();

			if constexpr (MemoryRaw::IsMovable<T>() == true)
			{
				MemoryRaw::MoveConstruct<T>(_rawPointer[_size], std::move(_rawPointer[_size - 1]));

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::MoveAssign<T>(_rawPointer[iter], std::move(_rawPointer[iter - 1]));
				}
			}
			else // 비효율적이지만 동작은 하도록 한다.
			{
				MemoryRaw::CopyConstruct<T>(_rawPointer[_size], _rawPointer[_size - 1]);

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::CopyAssign<T>(_rawPointer[iter], _rawPointer[iter - 1]);
				}
			}

			MemoryRaw::CopyAssign<T>(_rawPointer[at], newEntry);

			++_size;
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::Insert(T&& newEntry, const uint32 at) noexcept
	{
		if (_size <= at)
		{
			PushBack(newEntry);
		}
		else
		{
			ExpandCapacityIfNecessary();

			if constexpr (MemoryRaw::IsMovable<T>() == true)
			{
				MemoryRaw::MoveConstruct<T>(_rawPointer[_size], std::move(_rawPointer[_size - 1]));

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::MoveAssign<T>(_rawPointer[iter], std::move(_rawPointer[iter - 1]));
				}

				MemoryRaw::MoveAssign<T>(_rawPointer[at], std::move(newEntry));
			}
			else // 비효율적이지만 동작은 하도록 한다.
			{
				MemoryRaw::CopyConstruct<T>(_rawPointer[_size], _rawPointer[_size - 1]);

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::CopyAssign<T>(_rawPointer[iter], _rawPointer[iter - 1]);
				}

				MemoryRaw::CopyAssign<T>(_rawPointer[at], newEntry);
			}

			++_size;
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::Erase(const uint32 at) noexcept
	{
		if (_size == 0)
		{
			return;
		}

		if (at == _size - 1)
		{
			PopBack();
		}
		else
		{
			if constexpr (MemoryRaw::IsMovable<T>() == true)
			{
				for (uint32 iter = at + 1; iter < _size; ++iter)
				{
					MemoryRaw::MoveAssign<T>(_rawPointer[iter - 1], std::move(_rawPointer[iter]));
				}
			}
			else // 비효율적이지만 동작은 하도록 한다.
			{
				for (uint32 iter = at + 1; iter < _size; ++iter)
				{
					MemoryRaw::CopyAssign<T>(_rawPointer[iter - 1], _rawPointer[iter]);
				}
			}

			MemoryRaw::Destroy<T>(_rawPointer[_size - 1]);

			--_size;
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::ExpandCapacityIfNecessary() noexcept
	{
		if (_size == _capacity)
		{
			Reserve((_capacity == 0) ? kBaseCapacity : _capacity * 2);
		}
	}

	template<typename T>
	MINT_INLINE T& Vector<T>::Front() noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[0];
	}

	template<typename T>
	MINT_INLINE const T& Vector<T>::Front() const noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[0];
	}

	template<typename T>
	MINT_INLINE T& Vector<T>::Back() noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[_size - 1];
	}

	template<typename T>
	MINT_INLINE const T& Vector<T>::Back() const noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[_size - 1];
	}

	template<typename T>
	MINT_INLINE T& Vector<T>::At(const uint32 index) noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다.");
		return _rawPointer[Min(index, _size - 1)];
	}

	template<typename T>
	MINT_INLINE const T& Vector<T>::At(const uint32 index) const noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다.");
		return _rawPointer[Min(index, _size - 1)];
	}

	template<typename T>
	MINT_INLINE T* Vector<T>::Data() noexcept
	{
		return _rawPointer;
	}

	template<typename T>
	MINT_INLINE const T* Vector<T>::Data() const noexcept
	{
		return _rawPointer;
	}

	template<typename T>
	MINT_INLINE uint32 Vector<T>::Capacity() const noexcept
	{
		return _capacity;
	}

	template<typename T>
	MINT_INLINE uint32 Vector<T>::Size() const noexcept
	{
		return _size;
	}

	template<typename T>
	MINT_INLINE bool Vector<T>::IsEmpty() const noexcept
	{
		return (_size == 0);
	}

	template<typename T>
	MINT_INLINE Vector<T>::Iterator<T> Vector<T>::begin() noexcept
	{
		return Iterator(&_rawPointer[0]);
	}

	template<typename T>
	MINT_INLINE Vector<T>::Iterator<T> Vector<T>::end() noexcept
	{
		return Iterator(&_rawPointer[_size]);
	}
}


#endif // !_MINT_CONTAINER_VECTOR_HPP_
