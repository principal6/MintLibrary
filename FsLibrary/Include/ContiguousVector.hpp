#pragma once


#include <stdafx.h>
#include <FsLibrary/Include/ContiguousVector.h>

#include <FsLibrary/Include/Allocator.hpp>


namespace fs
{
	template<typename T>
	inline ContiguousVector<T>::ContiguousVector(fs::Memory::Allocator<T>& memoryAllocator, const uint32 capacity, const uint32 size)
		: _memoryAllocator{ memoryAllocator }
		, _memoryAccessor{ &_memoryAllocator }
		, _capacity{ capacity }
		, _size{ size }
	{
		__noop;
	}

	template<typename T>
	inline ContiguousVector<T>::ContiguousVector()
		: ContiguousVector(fs::Memory::Allocator<T>::getInstance(), 0, 0)
	{
		reserve(kDefaultCapacity);
	}

	template<typename T>
	inline ContiguousVector<T>::ContiguousVector(const std::initializer_list<T>& il)
		: ContiguousVector()
	{
		const uint32 count = static_cast<uint32>(il.size());
		resize(count);

		const T* const first = il.begin();
		for (uint32 index = 0; index < count; ++index)
		{
			this->operator[](index) = *(first + index);
		}
	}

	template<typename T>
	inline ContiguousVector<T>::ContiguousVector(const uint32 capacity)
		: ContiguousVector()
	{
		reserve(capacity);
	}

	template<typename T>
	inline ContiguousVector<T>::ContiguousVector(const ContiguousVector& rhs)
		: ContiguousVector(fs::Memory::Allocator<T>::getInstance(), rhs._capacity, rhs._size)
	{
		_memoryAccessor = rhs._memoryAccessor;
	}

	template<typename T>
	inline ContiguousVector<T>::ContiguousVector(ContiguousVector&& rhs) noexcept
		: ContiguousVector(fs::Memory::Allocator<T>::getInstance(), rhs._capacity, rhs._size)
	{
		_memoryAccessor = std::move(rhs._memoryAccessor);

		rhs._capacity = 0;
		rhs._size = 0;
	}

	template<typename T>
	inline ContiguousVector<T>::~ContiguousVector()
	{
		__noop;
	}

	template<typename T>
	inline ContiguousVector<T>& ContiguousVector<T>::operator=(const ContiguousVector& rhs)
	{
		if (this != &rhs)
		{
			_memoryAccessor = rhs._memoryAccessor;
			_capacity = rhs._capacity;
			_size = rhs._size;
		}
		return *this;
	}

	template<typename T>
	inline ContiguousVector<T>& ContiguousVector<T>::operator=(ContiguousVector&& rhs) noexcept
	{
		if (this != &rhs)
		{
			_memoryAccessor = std::move(rhs._memoryAccessor);
			_capacity = rhs._capacity;
			_size = rhs._size;

			rhs._capacity = 0;
			rhs._size = 0;
		}
		return *this;
	}

	template<typename T>
	inline void ContiguousVector<T>::reserve(const uint32 capacity)
	{
		if (_capacity < capacity)
		{
			_memoryAccessor = _memoryAllocator.reallocateArray(_memoryAccessor, capacity, true);

			_capacity = capacity;
		}
	}

	template<typename T>
	inline void ContiguousVector<T>::resize(const uint32 size)
	{
		if (_capacity < size)
		{
			reserve(size);
		}

		_size = size;
	}

	template<typename T>
	FS_INLINE void ContiguousVector<T>::clear()
	{
		_size = 0;
	}

	template<typename T>
	inline void ContiguousVector<T>::push_back(const T& value)
	{
		if (isFull() == true)
		{
			reserve(_capacity * 2);
		}

		_memoryAccessor.setMemory(&value, _size, 1);
		++_size;
	}

	template<typename T>
	inline void ContiguousVector<T>::push_back(T&& value)
	{
		if (isFull() == true)
		{
			reserve(_capacity * 2);
		}

		_memoryAccessor.setMemory(&value, _size, 1);
		++_size;
	}

	template<typename T>
	FS_INLINE void ContiguousVector<T>::pop_back()
	{
		if (0 < _size)
		{
			--_size;
		}
	}

	template<typename T>
	inline void ContiguousVector<T>::insert(const uint32 at, const T& value)
	{
		// _size 가 0 일 때 처리 및 at 이 end 이후를 가리킬 때 처리
		if (_size <= at)
		{
			return push_back(value);
		}

		if (isFull() == true)
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
	inline void ContiguousVector<T>::insert(const uint32 at, T&& value)
	{
		// _size 가 0 일 때 처리 및 at 이 end 이후를 가리킬 때 처리
		if (_size <= at)
		{
			return push_back(value);
		}

		if (isFull() == true)
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
	inline void ContiguousVector<T>::erase(const uint32 at)
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
	inline const T& ContiguousVector<T>::operator[](const uint32 index) const noexcept
	{
		//FS_ASSERT("김장원", index < _size, "index 가 size 를 벗어납니다!!!");
		//return _memoryAccessor.getMemory()[min(index, _size - 1)];
		return _memoryAccessor.getMemory()[index];
	}

	template<typename T>
	inline T& ContiguousVector<T>::operator[](const uint32 index) noexcept
	{
		//FS_ASSERT("김장원", index < _size, "index 가 size 를 벗어납니다!!!");
		//return _memoryAccessor.getMemoryXXX()[min(index, _size - 1)];
		return _memoryAccessor.getMemoryXXX()[index];
	}

	template<typename T>
	inline const T& ContiguousVector<T>::at(const uint32 index) const noexcept
	{
		FS_ASSERT("김장원", index < _size, "index 가 size 를 벗어납니다!!!");

		return this->operator[](min(index, _size - 1));
	}

	template<typename T>
	inline void ContiguousVector<T>::swap(const uint32 indexA, const uint32 indexB)
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
	FS_INLINE const bool ContiguousVector<T>::isFull() const noexcept
	{
		return _size == _capacity;
	}

	template<typename T>
	FS_INLINE const bool ContiguousVector<T>::empty() const noexcept
	{
		return (_size == 0);
	}

	template<typename T>
	FS_INLINE const uint32 ContiguousVector<T>::capacity() const noexcept
	{
		return _capacity;
	}

	template<typename T>
	FS_INLINE const uint32 ContiguousVector<T>::size() const noexcept
	{
		return _size;
	}

	template<typename T>
	inline const T& ContiguousVector<T>::front() const
	{
		FS_ASSERT("김장원", empty() == false, "비어 있는 ContiguousVector 입니다!");

		return _memoryAccessor.getMemory()[0];
	}

	template<typename T>
	inline const T& ContiguousVector<T>::back() const
	{
		FS_ASSERT("김장원", empty() == false, "비어 있는 ContiguousVector 입니다!");
		
		return _memoryAccessor.getMemory()[(0 < _size) ? _size - 1 : 0];
	}

}
