#pragma once


#include <stdafx.h>
#include <Container\Vector.h>

#include <Memory\MemoryAllocator2.hpp>


namespace fs
{
	template<typename T>
	MemoryAllocator2<T> Vector<T>::_memoryAllocator;

	template<typename T>
	inline Vector<T>::Vector()
		: _memoryAccessor{ &_memoryAllocator }
		, _capacity{ 0 }
		, _size{ 0 }
	{
		reserve(kDefaultCapacity);
	}

	template<typename T>
	inline Vector<T>::Vector(const uint32 capacity)
		: _memoryAccessor{ &_memoryAllocator }
		, _capacity{ 0 }
		, _size{ 0 }
	{
		reserve(capacity);
	}

	template<typename T>
	inline Vector<T>::~Vector()
	{
		__noop;
	}

	template<typename T>
	inline void Vector<T>::reserve(const uint32 capacity)
	{
		if (_capacity < capacity)
		{
			_memoryAccessor = _memoryAllocator.reallocateArray(_memoryAccessor, capacity, true);

			_capacity = capacity;
		}
	}

	template<typename T>
	inline void Vector<T>::resize(const uint32 size)
	{
		if (_capacity < size)
		{
			reserve(size);
		}

		_size = size;
	}

	template<typename T>
	FS_INLINE void Vector<T>::clear()
	{
		_size = 0;
	}

	template<typename T>
	inline void Vector<T>::push_back(const T& value)
	{
		if (hasEmptySlot() == false)
		{
			reserve(_capacity * 2);
		}

		_memoryAccessor.setMemory(&value, _size, 1);
		++_size;
	}

	template<typename T>
	inline void Vector<T>::push_back(T&& value)
	{
		if (hasEmptySlot() == false)
		{
			reserve(_capacity * 2);
		}

		_memoryAccessor.setMemory(&value, _size, 1);
		++_size;
	}

	template<typename T>
	FS_INLINE void Vector<T>::pop_back()
	{
		if (0 < _size)
		{
			--_size;
		}
	}

	template<typename T>
	inline void Vector<T>::insert(const uint32 at, const T& value)
	{
		// _size 가 0 일 때 처리 및 at 이 end 이후를 가리킬 때 처리
		if (_size <= at)
		{
			return push_back(value);
		}

		if (hasEmptySlot() == false)
		{
			reserve(_capacity * 2);
		}

		T* const rawMemory = _memoryAccessor.getMemoryXXX();
		const uint32 tailCount = _size - at;
		for (uint32 tailIter= 0; tailIter < tailCount; ++tailIter)
		{
			rawMemory[at + tailCount - tailIter] = rawMemory[at + tailCount - tailIter - 1];
		}
		rawMemory[at] = value;

		++_size;
	}

	template<typename T>
	inline void Vector<T>::insert(const uint32 at, T&& value)
	{
		// _size 가 0 일 때 처리 및 at 이 end 이후를 가리킬 때 처리
		if (_size <= at)
		{
			return push_back(value);
		}

		if (hasEmptySlot() == false)
		{
			reserve(_capacity * 2);
		}

		T* const rawMemory = _memoryAccessor.getMemoryXXX();
		const uint32 tailCount = _size - at;
		for (uint32 tailIter = 0; tailIter < tailCount; ++tailIter)
		{
			const uint32 offset = tailCount - tailIter;
			rawMemory[at + offset] = rawMemory[at + offset - 1];
		}
		rawMemory[at] = value;
		
		++_size;
	}

	template<typename T>
	inline void Vector<T>::erase(const uint32 at)
	{
		// _size 가 0 일 때 처리 및 at 이 end 이후를 가리킬 때 처리
		if (_size <= at)
		{
			return pop_back();
		}

		T* const rawMemory = _memoryAccessor.getMemoryXXX();
		const uint32 tailCount = _size - at;
		for (uint32 tailIter = 0; tailIter < tailCount; ++tailIter)
		{
			rawMemory[at + tailIter] = rawMemory[at + tailIter + 1];
		}

		--_size;
	}

	template<typename T>
	inline void Vector<T>::set(const uint32 index, const T& value)
	{
		if (empty() == true)
		{
			return;
		}

		_memoryAccessor.setMemory(&value, min(index, _size - 1), 1);
	}

	template<typename T>
	inline void Vector<T>::set(const uint32 index, T&& value)
	{
		if (empty() == true)
		{
			return;
		}

		_memoryAccessor.setMemory(&value, min(index, _size - 1), 1);
	}

	template<typename T>
	inline const T& Vector<T>::get(const uint32 index) const
	{
		FS_ASSERT("김장원", index < _size, "잘못된 index 입니다!");

		return _memoryAccessor.getMemory()[min(index, _size - 1)];
	}

	template<typename T>
	inline T& Vector<T>::get(const uint32 index)
	{
		FS_ASSERT("김장원", index < _size, "잘못된 index 입니다!");

		return _memoryAccessor.getMemoryXXX()[min(index, _size - 1)];
	}

	template<typename T>
	inline void Vector<T>::swap(const uint32 indexA, const uint32 indexB)
	{
		if (empty() == true)
		{
			return;
		}

		const uint32 clampedIndexA = min(indexA, _size - 1);
		const uint32 clampedIndexB = min(indexB, _size - 1);
		if (clampedIndexA == clampedIndexB)
		{
			return;
		}

		std::swap(*_memoryAccessor.getMemoryXXX()[clampedIndexA], *_memoryAccessor.getMemoryXXX()[clampedIndexB]);
	}

	template<typename T>
	FS_INLINE const bool Vector<T>::hasEmptySlot() const noexcept
	{
		return _size < _capacity;
	}

	template<typename T>
	FS_INLINE const bool Vector<T>::empty() const noexcept
	{
		return (_size == 0);
	}

	template<typename T>
	FS_INLINE const uint32 Vector<T>::capacity() const noexcept
	{
		return _capacity;
	}

	template<typename T>
	FS_INLINE const uint32 Vector<T>::size() const noexcept
	{
		return _size;
	}

	template<typename T>
	inline const T& Vector<T>::front() const
	{
		FS_ASSERT("김장원", empty() == false, "비어 있는 Vector 입니다!");

		return _memoryAccessor.getMemory()[0];
	}

	template<typename T>
	inline const T& Vector<T>::back() const
	{
		FS_ASSERT("김장원", empty() == false, "비어 있는 Vector 입니다!");
		
		return _memoryAccessor.getMemory()[(0 < _size) ? _size - 1 : 0];
	}

}
