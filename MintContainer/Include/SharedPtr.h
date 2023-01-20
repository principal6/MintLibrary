﻿#pragma once


#ifndef _MINT_CONTAINER_SHARED_PTR_H_
#define _MINT_CONTAINER_SHARED_PTR_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template<typename T>
	class SharedPtr;

	template<typename T>
	static SharedPtr<T> makeShared();
	
	template<typename T>
	static SharedPtr<T> makeShared(T&& rhs);


	class ReferenceCounter
	{
	public:
		ReferenceCounter()
			: _referenceCount{ 1 }
		{
			__noop;
		}
		ReferenceCounter(const ReferenceCounter& rhs) = delete;
		ReferenceCounter(ReferenceCounter&& rhs) noexcept
			: _referenceCount{ rhs._referenceCount }
		{
			rhs._referenceCount = 0;
		}
		~ReferenceCounter()
		{
			--_referenceCount;
		}
	public:
		ReferenceCounter& operator=(const ReferenceCounter& rhs) = delete;
		ReferenceCounter& operator=(ReferenceCounter&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_referenceCount = rhs._referenceCount;
				rhs._referenceCount = 0;
			}
			return *this;
		}
	public:
		MINT_INLINE void increaseReferenceCount() { ++_referenceCount; }
		MINT_INLINE void decreaseReferenceCount() { --_referenceCount; }
		MINT_INLINE int32 getReferenceCount() const { return _referenceCount; }
	private:
		int32 _referenceCount;
	};


	template<typename T>
	class SharedPtr
	{
		template<typename T>
		friend static SharedPtr<T> makeShared();

		template<typename T>
		friend static SharedPtr<T> makeShared(T&& rhs);

	public:
		SharedPtr()
			: _referenceCounter{ nullptr }
			, _rawPointer{ nullptr }
		{
			__noop;
		}
		SharedPtr(const SharedPtr& rhs)
			: _referenceCounter{ rhs._referenceCounter }
			, _rawPointer{ rhs._rawPointer }
		{
			if (_referenceCounter == nullptr)
			{
				return;
			}
			_referenceCounter->increaseReferenceCount();
		}
		SharedPtr(SharedPtr&& rhs)
			: _referenceCounter{ rhs._referenceCounter }
			, _rawPointer{ rhs._rawPointer }
		{
			rhs._referenceCounter = nullptr;
			rhs._rawPointer = nullptr;
		}
		~SharedPtr()
		{
			if (_referenceCounter == nullptr)
			{
				return;
			}
			_referenceCounter->decreaseReferenceCount();
			if (_referenceCounter->getReferenceCount() == 0)
			{
				MINT_DELETE(_referenceCounter);
				MINT_DELETE(_rawPointer);
			}
		}

	public:
		SharedPtr& operator=(const SharedPtr& rhs) noexcept
		{
			if (this != &rhs)
			{
				if (_referenceCounter != nullptr)
				{
					_referenceCounter->decreaseReferenceCount();
					if (_referenceCounter->getReferenceCount() == 0)
					{
						MINT_DELETE(_referenceCounter);
						MINT_DELETE(_rawPointer);
					}
				}

				_referenceCounter = rhs._referenceCounter;
				_rawPointer = rhs._rawPointer;

				if (_referenceCounter != nullptr)
				{
					_referenceCounter->increaseReferenceCount();
				}
			}
			return *this;
		}
		SharedPtr& operator=(SharedPtr&& rhs) noexcept
		{
			if (this != &rhs)
			{
				if (_referenceCounter != nullptr)
				{
					_referenceCounter->decreaseReferenceCount();
					if (_referenceCounter->getReferenceCount() == 0)
					{
						MINT_DELETE(_referenceCounter);
						MINT_DELETE(_rawPointer);
					}
				}

				_referenceCounter = rhs._referenceCounter;
				_rawPointer = rhs._rawPointer;

				rhs._rawPointer = nullptr;
				rhs._referenceCounter = nullptr;
			}
			return *this;
		}

	public:
		// this is really dangerous ...
		//T* operator&() noexcept { return _rawPointer; }
		T& operator*() noexcept { return *_rawPointer; }
		const T& operator*() const noexcept { return *_rawPointer; }
		T* operator->() noexcept { return _rawPointer; }
		const T* operator->() const noexcept { return _rawPointer; }

	public:
		constexpr uint64 size() { return sizeof(T); }
		bool isValid() const { return (_referenceCounter == nullptr ? false : _referenceCounter->getReferenceCount() > 0); }

	private:
		SharedPtr(T* const rawPointer)
			: _referenceCounter{ MINT_NEW(ReferenceCounter) }
			, _rawPointer{ rawPointer }
		{
			__noop;
		}

	private:
		ReferenceCounter* _referenceCounter;
		T* _rawPointer;
	};


	template<typename T>
	static SharedPtr<T> makeShared()
	{
		return SharedPtr<T>(MINT_NEW(T));
	}

	template<typename T>
	static SharedPtr<T> makeShared(T&& rhs)
	{
		return SharedPtr<T>(MINT_NEW(T, rhs));
	}
}


#endif // !_MINT_CONTAINER_SHARED_PTR_H_