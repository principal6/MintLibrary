#pragma once


#ifndef _MINT_CONTAINER_STACK_VECTOR_H_
#define _MINT_CONTAINER_STACK_VECTOR_H_


#include <MintContainer/Include/BasicVector.h>


namespace mint
{
	template<typename T, const uint32 kCapacity>
	class StackVectorStorage;

	template<typename T, const uint32 kCapacity>
	using StackVector = BasicVector<T, StackVectorStorage<T, kCapacity>>;

	template<typename T, const uint32 kCapacity>
	class StackVectorStorage final
	{
		friend BasicVector;

	public:
		StackVectorStorage();
		StackVectorStorage(const std::initializer_list<T>& initializerList);
		~StackVectorStorage();

	public:
		void Resize(const uint32 size);
		void PushBack(const T& entry);
		void PushBack(T&& entry);
		void PopBack();
		bool Insert(const uint32 at, const T& newEntry) noexcept;
		bool Insert(const uint32 at, T&& newEntry) noexcept;
		void Erase(const uint32 at) noexcept;

	public:
		T* Data() noexcept { return _array; }
		const T* Data() const noexcept { return _array; }

	public:
		constexpr uint32 Capacity() const { return kCapacity; }
		uint32 Size() const { return _size; }
		bool IsEmpty() const { return Size() == 0; }
		bool IsFull() const { return Size() == kCapacity; }

	private:
		T _array[kCapacity];
		uint32 _size;
	
	private:
		static_assert(kCapacity > 0, "kCapacity must be greater than 0");
		static constexpr bool kSupportsDynamicCapacity = false;
	};
}


#endif // !_MINT_CONTAINER_STACK_VECTOR_H_
