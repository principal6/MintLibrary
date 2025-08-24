#pragma once


#ifndef _MINT_CONTAINER_INLINE_VECTOR_H_
#define _MINT_CONTAINER_INLINE_VECTOR_H_


#include <MintContainer/Include/BasicVector.h>
#include <MintContainer/Include/MemoryRaw.h>


namespace mint
{
	template<typename T, const uint32 kCapacity>
	class InlineVectorStorage;

	template<typename T, const uint32 kCapacity>
	using InlineVector = BasicVector<T, InlineVectorStorage<T, kCapacity>>;

	template<typename T, const uint32 kCapacity>
	class InlineVectorStorage final
	{
		friend BasicVector;

	public:
		InlineVectorStorage();
		InlineVectorStorage(const std::initializer_list<T>& initializerList);
		~InlineVectorStorage();

	public:
		void Reserve(const uint32 capacity);
		void Resize(const uint32 size) requires (IsDefaultConstructible<T>() == true);
		void PushBack(const T& entry);
		void PushBack(T&& entry);
		void PopBack();
		void Insert(const T& newEntry, const uint32 at) noexcept;
		void Insert(T&& newEntry, const uint32 at) noexcept;
		void Erase(const uint32 at) noexcept;

	public:
		T* Data() noexcept { return _ptr; }
		const T* Data() const noexcept { return _ptr; }

	public:
		constexpr uint32 Capacity() const { return _capacity; }
		uint32 Size() const { return _size; }
		bool IsEmpty() const { return Size() == 0; }
		bool IsFull() const { return Size() == _capacity; }

	private:
		MINT_INLINE bool IsUsingHeap() const noexcept { return _ptr != __array; }
		uint32 _capacity;
		uint32 _size;
		T* _ptr = nullptr;
		T __array[kCapacity];

	private:
		static_assert(kCapacity > 0, "kCapacity must be greater than 0");
		static constexpr bool kSupportsDynamicCapacity = true;
	};
}


#endif // !_MINT_CONTAINER_INLINE_VECTOR_H_
