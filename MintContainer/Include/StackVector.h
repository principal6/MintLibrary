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
	public:
		StackVectorStorage();
		~StackVectorStorage();

	public:
		T& operator[](const uint32 index);
		const T& operator[](const uint32 index) const;

	public:
		void Resize(const uint32 size);
		void PushBack(const T& entry);
		void PushBack(T&& entry);
		void PopBack();
		void Clear();

	public:
		T& Front();
		const T& Front() const;
		T& Back();
		const T& Back() const;
		T& At(const uint32 index) noexcept;
		const T& At(const uint32 index) const noexcept;
		const T* Data() const noexcept { return _array; }

	public:
		constexpr uint32 Capacity() const { return kCapacity; }
		uint32 Size() const { return _size; }
		bool IsEmpty() const { return Size() == 0; }
		bool IsFull() const { return Size() == kCapacity; }

	private:
		T _array[kCapacity];
		uint32 _size;
	};
}


#endif // !_MINT_CONTAINER_STACK_VECTOR_H_
