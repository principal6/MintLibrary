#pragma once


#ifndef _MINT_CONTAINER_VECTOR_H_
#define _MINT_CONTAINER_VECTOR_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace std
{
	template <class _Elem>
	class initializer_list;
}


namespace mint
{
	template <typename T>
	class Vector final
	{
		static constexpr uint32 kBaseCapacity = 8;

	public:
		Vector();
		Vector(const uint32 size);
		Vector(const std::initializer_list<T>& initializerList);
		Vector(const Vector& rhs) noexcept;
		Vector(Vector&& rhs) noexcept;
		~Vector();

	public:
		Vector& operator=(const Vector& rhs) noexcept;
		Vector& operator=(Vector&& rhs) noexcept;

	public:
		T& operator[](const uint32 index) noexcept;
		const T& operator[](const uint32 index) const noexcept;

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

	public:
		template<typename T>
		class Iterator
		{
		public:
			Iterator(T* rawPointer) : _rawPointer{ rawPointer } { __noop; }

		public:
			bool operator==(const Iterator& rhs) const noexcept
			{
				return _rawPointer == rhs._rawPointer;
			}

			bool operator!=(const Iterator& rhs) const noexcept
			{
				return !(*this == rhs);
			}

			Iterator& operator++() noexcept
			{
				++_rawPointer;
				return *this;
			}

			T& operator*() noexcept
			{
				return *_rawPointer;
			}

		private:
			T* _rawPointer;
		};

		template<typename T>
		class ConstIterator
		{
		public:
			ConstIterator(const T* rawPointer) : _rawPointer{ rawPointer } { __noop; }

		public:
			bool operator==(const ConstIterator& rhs) const noexcept
			{
				return _rawPointer == rhs._rawPointer;
			}

			bool operator!=(const ConstIterator& rhs) const noexcept
			{
				return !(*this == rhs);
			}

			ConstIterator& operator++() noexcept
			{
				++_rawPointer;
				return *this;
			}

			const T& operator*() const noexcept
			{
				return *_rawPointer;
			}

		private:
			const T* _rawPointer;
		};

		Iterator<T> begin() noexcept;
		Iterator<T> end() noexcept;
		
		ConstIterator<T> begin() const noexcept;
		ConstIterator<T> end() const noexcept;

	private:
		T* _rawPointer;
		uint32 _capacity;
		uint32 _size;
	};
}


#endif // !_MINT_CONTAINER_VECTOR_H_
