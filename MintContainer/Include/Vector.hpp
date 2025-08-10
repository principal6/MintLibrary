#pragma once


#ifndef _MINT_CONTAINER_VECTOR_HPP_
#define _MINT_CONTAINER_VECTOR_HPP_


#include <initializer_list>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/MemoryRaw.hpp>


namespace mint
{
	template <typename T>
	inline VectorStorage<T>::VectorStorage()
		: _rawPointer{ nullptr }
		, _capacity{ 0 }
		, _size{ 0 }
	{
		__noop;
	}

	template<typename T>
	inline VectorStorage<T>::VectorStorage(const std::initializer_list<T>& initializerList)
		: VectorStorage()
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
	inline VectorStorage<T>::VectorStorage(const VectorStorage& rhs) noexcept
		: VectorStorage()
	{
		Reserve(rhs._size);

		for (uint32 index = 0; index < rhs._size; ++index)
		{
			PushBack(rhs._rawPointer[index]);
		}
	}

	template<typename T>
	inline VectorStorage<T>::VectorStorage(VectorStorage&& rhs) noexcept
		: _rawPointer{ rhs._rawPointer }
		, _capacity{ rhs._capacity }
		, _size{ rhs._size }
	{
		rhs._rawPointer = nullptr;
		rhs._capacity = 0;
		rhs._size = 0;
	}

	template<typename T>
	inline VectorStorage<T>::~VectorStorage()
	{
		DestroyAll();

		MemoryRaw::DeallocateMemory<T>(_rawPointer);
	}

	template<typename T>
	MINT_INLINE VectorStorage<T>& VectorStorage<T>::operator=(const VectorStorage<T>& rhs) noexcept
	{
		if (this != &rhs)
		{
			DestroyAll();

			Reserve(rhs._size);

			for (uint32 index = 0; index < rhs._size; ++index)
			{
				PushBack(rhs._rawPointer[index]);
			}
		}
		return *this;
	}

	template<typename T>
	MINT_INLINE VectorStorage<T>& VectorStorage<T>::operator=(VectorStorage<T>&& rhs) noexcept
	{
		if (this != &rhs)
		{
			DestroyAll();

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
	MINT_INLINE void VectorStorage<T>::Reserve(const uint32 capacity) noexcept
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
	MINT_INLINE void VectorStorage<T>::Resize(const uint32 size) noexcept
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
	MINT_INLINE void VectorStorage<T>::ShrinkToFit() noexcept
	{
		if (_capacity <= _size)
		{
			return;
		}

		if (_size == 0)
		{
			MemoryRaw::DeallocateMemory<T>(_rawPointer);
			_capacity = 0;
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
	MINT_INLINE void VectorStorage<T>::PushBack(const T& newEntry) noexcept
	{
		ExpandCapacityIfNecessary();

		MemoryRaw::CopyConstruct<T>(_rawPointer[_size], newEntry);

		++_size;
	}

	template<typename T>
	MINT_INLINE void VectorStorage<T>::PushBack(T&& newEntry) noexcept
	{
		ExpandCapacityIfNecessary();

		MemoryRaw::MoveConstruct<T>(_rawPointer[_size], std::move(newEntry));

		++_size;
	}

	template<typename T>
	MINT_INLINE void VectorStorage<T>::PopBack() noexcept
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
	MINT_INLINE void VectorStorage<T>::Insert(const T& newEntry, const uint32 at) noexcept
	{
		if (_size <= at)
		{
			PushBack(newEntry);
		}
		else
		{
			ExpandCapacityIfNecessary();

			if constexpr (IsMovable<T>() == true)
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
	MINT_INLINE void VectorStorage<T>::Insert(T&& newEntry, const uint32 at) noexcept
	{
		if (_size <= at)
		{
			PushBack(newEntry);
		}
		else
		{
			ExpandCapacityIfNecessary();

			if constexpr (IsMovable<T>() == true)
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
	MINT_INLINE void VectorStorage<T>::Erase(const uint32 at) noexcept
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
			if constexpr (IsMovable<T>() == true)
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
	MINT_INLINE void VectorStorage<T>::ExpandCapacityIfNecessary() noexcept
	{
		if (_size == _capacity)
		{
			Reserve((_capacity == 0) ? kBaseCapacity : _capacity * 2);
		}
	}

	template<typename T>
	MINT_INLINE void VectorStorage<T>::DestroyAll() noexcept
	{
		if (_size == 0)
		{
			return;
		}

		for(uint32 index = 0; index < _size; ++index)
		{
			MemoryRaw::Destroy<T>(_rawPointer[index]);
		}
		_size = 0;
	}
}


#endif // !_MINT_CONTAINER_VECTOR_HPP_
