#pragma once


#ifndef _MINT_CONTAINER_VECTOR_H_
#define _MINT_CONTAINER_VECTOR_H_


#include <MintContainer/Include/BasicVector.h>


namespace std
{
	template <class _Elem>
	class initializer_list;
}


namespace mint
{
	template <typename T>
	class VectorStorage;

	template<typename T>
	using Vector = BasicVector<T, VectorStorage<T>>;

	template <typename T>
	class VectorStorage final : public BasicVectorStorage<T>
	{
	public:
		VectorStorage();
		VectorStorage(const std::initializer_list<T>& initializerList);
		VectorStorage(const VectorStorage& rhs) noexcept;
		VectorStorage(VectorStorage&& rhs) noexcept;
		~VectorStorage();

	public:
		VectorStorage& operator=(const VectorStorage& rhs) noexcept;
		VectorStorage& operator=(VectorStorage&& rhs) noexcept;

	public:
		void Reserve(const uint32 capacity) noexcept;
		void Resize(const uint32 size) noexcept; // default-constructible 의 경우에만 호출 가능하다!
		void ShrinkToFit() noexcept;

	public:
		void PushBack(const T& newEntry);
		void PushBack(T&& newEntry);
		void PopBack() noexcept;
		bool Insert(const uint32 at, const T& newEntry);
		bool Insert(const uint32 at, T&& newEntry);
		void Erase(const uint32 at) noexcept;
		void Clear() noexcept;

	private:
		void ExpandCapacityIfNecessary() noexcept;

	public:
		MINT_INLINE virtual T* Data() noexcept override final { return _rawPointer; }
		MINT_INLINE virtual const T* Data() const noexcept override final { return _rawPointer; }
		MINT_INLINE virtual uint32 Capacity() const noexcept override final { return _capacity; }
		MINT_INLINE virtual uint32 Size() const noexcept override final { return _size; }

	public:
		static constexpr bool kSupportsDynamicCapacity = true;

	private:
		T* _rawPointer;
		uint32 _capacity;
		uint32 _size;

	private:
		static constexpr uint32 kBaseCapacity = 8;
	};
}


#endif // !_MINT_CONTAINER_VECTOR_H_
