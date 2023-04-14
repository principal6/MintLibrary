#pragma once


#ifndef _MINT_CONTAINER_QUEUE_H_
#define _MINT_CONTAINER_QUEUE_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template <typename T>
	class Queue
	{
	public:
		Queue();
		~Queue();

	public:
		void Reserve(uint32 capacity) noexcept;

	public:
		MINT_INLINE uint32 Size() const noexcept { return _size; }
		MINT_INLINE uint32 Capacity() const noexcept { return _capacity; }
		MINT_INLINE bool IsEmpty() const noexcept { return (_size == 0); }

	public:
		void Push(const T& newEntry) noexcept;
		void Push(T&& newEntry) noexcept;
		void Pop() noexcept;
		void Flush() noexcept;

	public:
		T& Peek() noexcept;
		const T& Peek() const noexcept;
	
	public:
		MINT_INLINE uint32 HeadAt() const { return _headAt; }
		MINT_INLINE uint32 TailAt() const { return _tailAt; }

	private:
		void SaveBackup(T*& backUpPointer) noexcept;
		void RestoreBackup(const T* const backUpPointer) noexcept;

	private:
		T* _rawPointer;
		uint32 _size;
		uint32 _capacity;
		uint32 _headAt;
		uint32 _tailAt;
	};
}


#endif // !_MINT_CONTAINER_QUEUE_H_
