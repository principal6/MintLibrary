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
	class StackVectorStorage final : public BasicVectorStorage<T>
	{
	public:
		StackVectorStorage();
		StackVectorStorage(const std::initializer_list<T>& initializerList);
		~StackVectorStorage();

	public:
		void Resize(uint32 size);
	
	public:
		void PushBack(const T& entry);
		void PushBack(T&& entry);
		void PopBack() noexcept;
		bool Insert(const uint32 at, const T& newEntry);
		bool Insert(const uint32 at, T&& newEntry);
		void Erase(const uint32 at) noexcept;
		void Clear() noexcept;

	public:
		MINT_INLINE virtual T* Data() noexcept { return reinterpret_cast<T*>(_array); }
		MINT_INLINE virtual const T* Data() const noexcept { return reinterpret_cast<const T*>(_array); }
		MINT_INLINE virtual constexpr uint32 Capacity() const noexcept override final { return kCapacity; }
		MINT_INLINE virtual uint32 Size() const noexcept override final { return _size; }

	public:
		static constexpr bool kSupportsDynamicCapacity = false;

	private:
		alignas(T) byte _array[sizeof(T) * kCapacity];
		uint32 _size;
	
	private:
		static_assert(kCapacity > 0, "kCapacity must be greater than 0");
	};
}


#endif // !_MINT_CONTAINER_STACK_VECTOR_H_
