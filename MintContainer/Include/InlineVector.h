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
	class InlineVectorStorage final : public BasicVectorStorage<T>
	{
	public:
		InlineVectorStorage();
		InlineVectorStorage(const std::initializer_list<T>& initializerList);
		~InlineVectorStorage();

	public:
		void Reserve(const uint32 newCapacity);
		void Resize(const uint32 newSize) requires (IsDefaultConstructible<T>() == true);
	
	public:
		void PushBack(const T& entry);
		void PushBack(T&& entry);
		void PopBack() noexcept;
		bool Insert(const uint32 at, const T& newEntry);
		bool Insert(const uint32 at, T&& newEntry);
		void Erase(const uint32 at) noexcept;
		void Clear() noexcept;

	public:
		MINT_INLINE virtual T* Data() noexcept override final { return _ptr; }
		MINT_INLINE virtual const T* Data() const noexcept override final { return _ptr; }
		MINT_INLINE virtual constexpr uint32 Capacity() const noexcept override final { return _capacity; }
		MINT_INLINE virtual uint32 Size() const noexcept override final { return _size; }

	private:
		MINT_INLINE bool IsUsingHeap() const noexcept { return _ptr != reinterpret_cast<const T*>(__array); }

	public:
		static constexpr bool kSupportsDynamicCapacity = true;

	private:
		uint32 _capacity;
		uint32 _size;
		T* _ptr = nullptr;
		alignas(T) byte __array[sizeof(T) * kCapacity];

	private:
		static_assert(kCapacity > 0, "kCapacity must be greater than 0");
	};
}


#endif // !_MINT_CONTAINER_INLINE_VECTOR_H_
