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
		void Clear() noexcept;
		void PushBack(const T& newEntry) noexcept;
		void PushBack(T&& newEntry) noexcept;
		void PopBack() noexcept;
		void Insert(const T& newEntry, const uint32 at) noexcept;
		void Insert(T&& newEntry, const uint32 at) noexcept;
		void Erase(const uint32 at) noexcept;

	private:
		void ExpandCapacityIfNecessary() noexcept;

	public:
		T& Front() noexcept;
		const T& Front() const noexcept;
		T& Back() noexcept;
		const T& Back() const noexcept;
		T& At(const uint32 index) noexcept;
		const T& At(const uint32 index) const noexcept;
		T* Data() noexcept;
		const T* Data() const noexcept;

	public:
		uint32 Capacity() const noexcept;
		uint32 Size() const noexcept;
		bool IsEmpty() const noexcept;

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
