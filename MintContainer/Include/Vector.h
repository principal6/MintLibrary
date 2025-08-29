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
	class VectorStorage final
	{
		friend BasicVector;

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
		void PushBack(const T& newEntry) noexcept;
		void PushBack(T&& newEntry) noexcept;
		void PopBack() noexcept;
		bool Insert(const uint32 at, const T& newEntry) noexcept;
		bool Insert(const uint32 at, T&& newEntry) noexcept;
		void Erase(const uint32 at) noexcept;

	private:
		void ExpandCapacityIfNecessary() noexcept;
		void DestroyAll() noexcept;

	public:
		MINT_INLINE T* Data() noexcept { return _rawPointer; }
		MINT_INLINE const T* Data() const noexcept { return _rawPointer; }

	public:
		MINT_INLINE uint32 Capacity() const noexcept { return _capacity; }
		MINT_INLINE uint32 Size() const noexcept { return _size; }
		MINT_INLINE bool IsEmpty() const noexcept { return _size == 0; }

	private:
		T* _rawPointer;
		uint32 _capacity;
		uint32 _size;

	private:
		static constexpr bool kSupportsDynamicCapacity = true;
		static constexpr uint32 kBaseCapacity = 8;
	};
}


#endif // !_MINT_CONTAINER_VECTOR_H_
