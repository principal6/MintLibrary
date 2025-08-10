#pragma once


#ifndef _MINT_CONTAINER_BASIC_VECTOR_H_
#define _MINT_CONTAINER_BASIC_VECTOR_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace std
{
	template <class _Elem>
	class initializer_list;
}


namespace mint
{
	template<typename T, class Storage>
	class BasicVector final
	{
	public:
		MINT_INLINE BasicVector() = default;
		MINT_INLINE BasicVector(const std::initializer_list<T>& initializerList) : _storage{ initializerList } {}
		MINT_INLINE BasicVector(const uint32 size) { _storage.Resize(size); }
		MINT_INLINE BasicVector(const BasicVector& rhs) = default;
		MINT_INLINE BasicVector(BasicVector&& rhs) noexcept = default;
		MINT_INLINE ~BasicVector() = default;

	public:
		BasicVector& operator=(const BasicVector& rhs) noexcept = default;
		BasicVector& operator=(BasicVector&& rhs) noexcept = default;

	public:
		MINT_INLINE T& operator[](const uint32 index) 
		{
			MINT_ASSERT(index < Size(), "범위를 벗어난 접근입니다. [index: %d / size: %d]", index, Size());
			return _storage.Data()[index];
		}
		MINT_INLINE const T& operator[](const uint32 index) const 
		{
			MINT_ASSERT(index < Size(), "범위를 벗어난 접근입니다. [index: %d / size: %d]", index, Size());
			return _storage.Data()[index];
		}

	public:
		MINT_INLINE void Reserve(const uint32 capacity) requires(Storage::kSupportsDynamicCapacity) { _storage.Reserve(capacity); }
		MINT_INLINE void Resize(const uint32 size) { _storage.Resize(size); }
		MINT_INLINE void ShrinkToFit() noexcept requires(Storage::kSupportsDynamicCapacity) { _storage.ShrinkToFit(); }
		MINT_INLINE void PushBack(const T& entry) { _storage.PushBack(entry); }
		MINT_INLINE void PushBack(T&& entry) { _storage.PushBack(std::move(entry)); }
		MINT_INLINE void PopBack() { _storage.PopBack(); }
		MINT_INLINE void Insert(const T& newEntry, const uint32 at) noexcept { _storage.Insert(newEntry, at); }
		MINT_INLINE void Insert(T&& newEntry, const uint32 at) noexcept { _storage.Insert(std::move(newEntry), at); }
		MINT_INLINE void Erase(const uint32 at) noexcept { _storage.Erase(at); }
		MINT_INLINE void Clear() { _storage.Clear(); }

	public:
		MINT_INLINE T* Data() noexcept { return _storage.Data(); }
		MINT_INLINE const T* Data() const noexcept { return _storage.Data(); }
		MINT_INLINE T& Front() { return _storage.Front(); }
		MINT_INLINE const T& Front() const { return _storage.Front(); }
		MINT_INLINE T& Back() { return _storage.Back(); }
		MINT_INLINE const T& Back() const { return _storage.Back(); }
		MINT_INLINE T& At(const uint32 index) noexcept { return _storage.At(index); }
		MINT_INLINE const T& At(const uint32 index) const noexcept { return _storage.At(index); }

	public:
		MINT_INLINE constexpr uint32 Capacity() const { return _storage.Capacity(); }
		MINT_INLINE uint32 Size() const { return _storage.Size(); }
		MINT_INLINE bool IsEmpty() const { return _storage.Size() == 0; }
		MINT_INLINE bool IsFull() const { return _storage.Size() == _storage.Capacity(); }

	public:
		// TODO: Iterator, ConstIterator 를 클래스로 만들기???
		using Iterator = T*;
		using ConstIterator = const T*;

		MINT_INLINE Iterator begin() noexcept { return _storage.Data(); }
		MINT_INLINE Iterator end() noexcept { return _storage.Data() + _storage.Size(); }
		MINT_INLINE ConstIterator begin() const noexcept { return _storage.Data(); }
		MINT_INLINE ConstIterator end() const noexcept { return _storage.Data() + _storage.Size(); }

	private:
		Storage _storage;
	};
}
#endif // !_MINT_CONTAINER_BASIC_VECTOR_H_
