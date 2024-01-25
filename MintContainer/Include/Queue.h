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
		const T& Get(uint32 index) const noexcept;

	public:
		MINT_INLINE uint32 HeadAt() const { return _headAt; }
		MINT_INLINE uint32 TailAt() const { return _tailAt; }

	public:
		class ConstIterator
		{
		public:
			ConstIterator(const Queue<T>& queue, uint32 at, bool isEnd) : _queue{ queue }, _at{ at }, _isEnd{ isEnd } { __noop; }

		public:
			bool operator==(const ConstIterator& rhs) const noexcept
			{
				return &_queue == &rhs._queue && (_isEnd == rhs._isEnd || _at == rhs._at);
			}
			bool operator!=(const ConstIterator& rhs) const noexcept
			{
				return !(*this == rhs);
			}
			ConstIterator& operator++() noexcept
			{
				++_at;
				if (_at >= _queue._tailAt)
				{
					_isEnd = true;
				}
				if (_at >= _queue._capacity)
				{
					_at = 0;
				}
				return *this;
			}
			const T& Get() const noexcept
			{
				const T& value = _queue._rawPointer[_at];
				return value;
			}

		private:
			const Queue<T>& _queue;
			uint32 _at;
			bool _isEnd;
		};
		ConstIterator begin() const;
		ConstIterator end() const;

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
