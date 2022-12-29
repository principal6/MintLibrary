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
		resize(size);
	}

	template<typename T>
	inline Vector<T>::Vector(const std::initializer_list<T>& initializerList)
		: Vector()
	{
		const uint32 count = static_cast<uint32>(initializerList.size());
		reserve(count);

		const T* const rawPointer = initializerList.begin();
		for (uint32 index = 0; index < count; ++index)
		{
			push_back(rawPointer[index]);
		}
	}

	template<typename T>
	inline Vector<T>::Vector(const Vector& rhs) noexcept
		: Vector()
	{
		reserve(rhs._size);

		for (uint32 index = 0; index < rhs._size; ++index)
		{
			push_back(rhs._rawPointer[index]);
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
		clear();

		MemoryRaw::deallocateMemory<T>(_rawPointer);
	}

	template<typename T>
	MINT_INLINE Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) noexcept
	{
		if (this != &rhs)
		{
			clear();

			reserve(rhs._size);

			for (uint32 index = 0; index < rhs._size; ++index)
			{
				push_back(rhs._rawPointer[index]);
			}
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept
	{
		if (this != &rhs)
		{
			clear();

			MemoryRaw::deallocateMemory<T>(_rawPointer);

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
	MINT_INLINE void Vector<T>::reserve(const uint32 capacity) noexcept
	{
		if (capacity <= _capacity)
		{
			return;
		}

		// 잦은 reserve 시 성능 최적화!!!
		_capacity = mint::max(capacity, _capacity * 2);

		if (_size == 0)
		{
			MemoryRaw::deallocateMemory<T>(_rawPointer);
			_rawPointer = MemoryRaw::allocateMemory<T>(_capacity);
		}
		else
		{
			T* temp = MemoryRaw::allocateMemory<T>(_size);
			MemoryRaw::moveMemory<T>(temp, _rawPointer, _size);

			MemoryRaw::deallocateMemory<T>(_rawPointer);
			_rawPointer = MemoryRaw::allocateMemory<T>(_capacity);

			MemoryRaw::moveMemory<T>(_rawPointer, temp, _size);
			MemoryRaw::deallocateMemory<T>(temp);
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::resize(const uint32 size) noexcept
	{
		reserve(size);

		if (_size < size)
		{
			for (uint32 index = _size; index < size; ++index)
			{
				MemoryRaw::construct(_rawPointer[index]);
			}
		}
		else if (size < _size)
		{
			for (uint32 index = size; index < _size; ++index)
			{
				pop_back();
			}
		}

		_size = size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::shrink_to_fit() noexcept
	{
		if (_capacity <= _size)
		{
			return;
		}

		T* temp = MemoryRaw::allocateMemory<T>(_size);
		MemoryRaw::moveMemory<T>(temp, _rawPointer, _size);

		MemoryRaw::deallocateMemory<T>(_rawPointer);
		_rawPointer = MemoryRaw::allocateMemory<T>(_size);

		MemoryRaw::moveMemory<T>(_rawPointer, temp, _size);
		MemoryRaw::deallocateMemory<T>(temp);

		_capacity = _size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::clear() noexcept
	{
		while (empty() == false)
		{
			pop_back();
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::push_back(const T& newEntry) noexcept
	{
		expandCapacityIfNecessary();

		MemoryRaw::copyConstruct<T>(_rawPointer[_size], newEntry);

		++_size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::push_back(T&& newEntry) noexcept
	{
		expandCapacityIfNecessary();

		MemoryRaw::moveConstruct<T>(_rawPointer[_size], std::move(newEntry));

		++_size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::pop_back() noexcept
	{
		if (_size == 0)
		{
			return;
		}

		// valid 한 범위 내 [0, _size - 1] element 는
		// 모두 ctor 의 호출이 보장되었으므로
		// 반드시 destroy() 를 호출해야 한다.
		MemoryRaw::destroy<T>(_rawPointer[_size - 1]);

		--_size;
	}

	template<typename T>
	MINT_INLINE void Vector<T>::insert(const T& newEntry, const uint32 at) noexcept
	{
		if (_size <= at)
		{
			push_back(newEntry);
		}
		else
		{
			expandCapacityIfNecessary();

			if constexpr (MemoryRaw::isMovable<T>() == true)
			{
				MemoryRaw::moveConstruct<T>(_rawPointer[_size], std::move(_rawPointer[_size - 1]));

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::moveAssign<T>(_rawPointer[iter], std::move(_rawPointer[iter - 1]));
				}
			}
			else // 비효율적이지만 동작은 하도록 한다.
			{
				MemoryRaw::copyConstruct<T>(_rawPointer[_size], _rawPointer[_size - 1]);

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::copyAssign<T>(_rawPointer[iter], _rawPointer[iter - 1]);
				}
			}

			MemoryRaw::copyAssign<T>(_rawPointer[at], newEntry);

			++_size;
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::insert(T&& newEntry, const uint32 at) noexcept
	{
		if (_size <= at)
		{
			push_back(newEntry);
		}
		else
		{
			expandCapacityIfNecessary();

			if constexpr (MemoryRaw::isMovable<T>() == true)
			{
				MemoryRaw::moveConstruct<T>(_rawPointer[_size], std::move(_rawPointer[_size - 1]));

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::moveAssign<T>(_rawPointer[iter], std::move(_rawPointer[iter - 1]));
				}

				MemoryRaw::moveAssign<T>(_rawPointer[at], std::move(newEntry));
			}
			else // 비효율적이지만 동작은 하도록 한다.
			{
				MemoryRaw::copyConstruct<T>(_rawPointer[_size], _rawPointer[_size - 1]);

				for (uint32 iter = _size - 1; iter > at; --iter)
				{
					MemoryRaw::copyAssign<T>(_rawPointer[iter], _rawPointer[iter - 1]);
				}

				MemoryRaw::copyAssign<T>(_rawPointer[at], newEntry);
			}

			++_size;
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::erase(const uint32 at) noexcept
	{
		if (_size == 0)
		{
			return;
		}

		if (at == _size - 1)
		{
			pop_back();
		}
		else
		{
			if constexpr (MemoryRaw::isMovable<T>() == true)
			{
				for (uint32 iter = at + 1; iter < _size; ++iter)
				{
					MemoryRaw::moveAssign<T>(_rawPointer[iter - 1], std::move(_rawPointer[iter]));
				}
			}
			else // 비효율적이지만 동작은 하도록 한다.
			{
				for (uint32 iter = at + 1; iter < _size; ++iter)
				{
					MemoryRaw::copyAssign<T>(_rawPointer[iter - 1], _rawPointer[iter]);
				}
			}

			MemoryRaw::destroy<T>(_rawPointer[_size - 1]);

			--_size;
		}
	}

	template<typename T>
	MINT_INLINE void Vector<T>::expandCapacityIfNecessary() noexcept
	{
		if (_size == _capacity)
		{
			reserve((_capacity == 0) ? kBaseCapacity : _capacity * 2);
		}
	}

	template<typename T>
	MINT_INLINE T& Vector<T>::front() noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[0];
	}

	template<typename T>
	MINT_INLINE const T& Vector<T>::front() const noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[0];
	}

	template<typename T>
	MINT_INLINE T& Vector<T>::back() noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[_size - 1];
	}

	template<typename T>
	MINT_INLINE const T& Vector<T>::back() const noexcept
	{
		MINT_ASSERT(_size > 0, "범위를 벗어난 접근입니다.");
		return _rawPointer[_size - 1];
	}

	template<typename T>
	MINT_INLINE T& Vector<T>::at(const uint32 index) noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다.");
		return _rawPointer[mint::min(index, _size - 1)];
	}

	template<typename T>
	MINT_INLINE const T& Vector<T>::at(const uint32 index) const noexcept
	{
		MINT_ASSERT(index < _size, "범위를 벗어난 접근입니다.");
		return _rawPointer[mint::min(index, _size - 1)];
	}

	template<typename T>
	MINT_INLINE T* Vector<T>::data() noexcept
	{
		return _rawPointer;
	}

	template<typename T>
	MINT_INLINE const T* Vector<T>::data() const noexcept
	{
		return _rawPointer;
	}

	template<typename T>
	MINT_INLINE uint32 Vector<T>::capacity() const noexcept
	{
		return _capacity;
	}

	template<typename T>
	MINT_INLINE uint32 Vector<T>::size() const noexcept
	{
		return _size;
	}

	template<typename T>
	MINT_INLINE bool Vector<T>::empty() const noexcept
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
