#pragma once


#ifndef _MINT_CONTAINER_SHARED_PTR_H_
#define _MINT_CONTAINER_SHARED_PTR_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template<typename T>
	class SharedPtr;

	template<typename T>
	class SharedPtrViewer;

	template<typename T>
	static SharedPtr<T> MakeShared();

	template<typename T>
	static SharedPtr<T> MakeShared(T&& rhs);


	class SharedRefCounter
	{
	public:
		SharedRefCounter()
			: _strongRefCount{ 0 }
			, _weakRefCount{ 0 }
		{
			__noop;
		}
		SharedRefCounter(const SharedRefCounter& rhs) = delete;
		SharedRefCounter(SharedRefCounter&& rhs) noexcept
			: _strongRefCount{ rhs._strongRefCount }
			, _weakRefCount{ rhs._weakRefCount }
		{
			rhs._strongRefCount = 0;
			rhs._weakRefCount = 0;
		}
		~SharedRefCounter() { __noop; }
		SharedRefCounter& operator=(const SharedRefCounter& rhs) = delete;
		SharedRefCounter& operator=(SharedRefCounter&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_strongRefCount = rhs._strongRefCount;
				_weakRefCount = rhs._weakRefCount;

				rhs._strongRefCount = 0;
				rhs._weakRefCount = 0;
			}
			return *this;
		}
	public:
		MINT_INLINE void IncreaseStrongRefCount() { ++_strongRefCount; }
		MINT_INLINE void IncreaseWeakRefCount() { ++_weakRefCount; }
		MINT_INLINE void DecreaseStrongRefCount() { --_strongRefCount; }
		MINT_INLINE void DecreaseWeakRefCount() { --_weakRefCount; }
		MINT_INLINE int32 GetStrongRefCount() const { return _strongRefCount; }
		MINT_INLINE int32 GetWeakRefCount() const { return _weakRefCount; }
	private:
		int32 _strongRefCount;
		int32 _weakRefCount;
	};


	template<typename T>
	class SharedPtr
	{
		template<typename U>
		friend class SharedPtr;

		template<typename T>
		friend class SharedPtrViewer;

		template<typename T>
		friend static SharedPtr<T> MakeShared();

		template<typename T>
		friend static SharedPtr<T> MakeShared(const T& rhs);

		template<typename T, typename U>
		friend static SharedPtr<T> MakeShared(const U& rhs);

		template<typename T>
		friend static SharedPtr<T> MakeShared(T&& rhs);

	public:
		SharedPtr() = default;
		SharedPtr(const SharedPtr& rhs)
			: _sharedRefCounter{ rhs._sharedRefCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->IncreaseStrongRefCount();
			}
		}
		SharedPtr(SharedPtr&& rhs) noexcept
			: _sharedRefCounter{ rhs._sharedRefCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			rhs._sharedRefCounter = nullptr;
			rhs._rawPtr = nullptr;
		}
		template<typename U>
		SharedPtr(const SharedPtr<U>& rhs)
			: _sharedRefCounter{ rhs._sharedRefCounter }
			, _rawPtr{ dynamic_cast<T*>(rhs._rawPtr) }
		{
			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->IncreaseStrongRefCount();
			}
		}
		~SharedPtr()
		{
			DecreaseReferenceCount();
		}
		SharedPtr& operator=(const SharedPtr& rhs)
		{
			if (this == &rhs)
			{
				return *this;
			}

			DecreaseReferenceCount();

			_sharedRefCounter = rhs._sharedRefCounter;
			_rawPtr = rhs._rawPtr;

			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->IncreaseStrongRefCount();
			}
			return *this;
		}
		SharedPtr& operator=(SharedPtr&& rhs) noexcept
		{
			if (this == &rhs)
			{
				return *this;
			}

			DecreaseReferenceCount();

			_sharedRefCounter = rhs._sharedRefCounter;
			_rawPtr = rhs._rawPtr;

			rhs._sharedRefCounter = nullptr;
			rhs._rawPtr = nullptr;
			return *this;
		}

	public:
		// this is really dangerous ...
		//T* operator&() noexcept { return _rawPtr; }
		T& operator*() noexcept { return *_rawPtr; }
		T& operator*() const noexcept { return *_rawPtr; }
		T* operator->() noexcept { return _rawPtr; }
		T* operator->() const noexcept { return _rawPtr; }

	public:
		bool IsValid() const { return (_sharedRefCounter == nullptr ? false : _sharedRefCounter->GetStrongRefCount() != 0); }
		void Clear()
		{
			DecreaseReferenceCount();

			_sharedRefCounter = nullptr;
			_rawPtr = nullptr;
		}
		T* Get() { return _rawPtr; }
		const T* Get() const { return _rawPtr; }

	private:
		SharedPtr(T* const rawPointer)
			: _sharedRefCounter{ MINT_NEW(SharedRefCounter) }
			, _rawPtr{ rawPointer }
		{
			_sharedRefCounter->IncreaseStrongRefCount();
		}

		void DecreaseReferenceCount();

	private:
		SharedRefCounter* _sharedRefCounter = nullptr;
		T* _rawPtr = nullptr;
	};

	template<typename T>
	class SharedPtrViewer
	{
	public:
		SharedPtrViewer() = default;
		SharedPtrViewer(const SharedPtr<T>& rhs)
			: _sharedRefCounter{ rhs._sharedRefCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->IncreaseWeakRefCount();
			}
		}
		SharedPtrViewer(const SharedPtrViewer& rhs)
			: _sharedRefCounter{ rhs._sharedRefCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->IncreaseWeakRefCount();
			}
		}
		SharedPtrViewer(SharedPtrViewer&& rhs)
			: _sharedRefCounter{ rhs._sharedRefCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			rhs._sharedRefCounter = nullptr;
			rhs._rawPtr = nullptr;
		}
		~SharedPtrViewer()
		{
			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->DecreaseWeakRefCount();

				if (_sharedRefCounter->GetStrongRefCount() == 0 && _sharedRefCounter->GetWeakRefCount() == 0)
				{
					MINT_DELETE(_sharedRefCounter);
				}
			}
		}
		SharedPtrViewer& operator=(const SharedPtr<T>& rhs) noexcept
		{
			if (_sharedRefCounter == rhs._sharedRefCounter)
			{
				return *this;
			}

			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->DecreaseWeakRefCount();

				if (_sharedRefCounter->GetStrongRefCount() == 0 && _sharedRefCounter->GetWeakRefCount() == 0)
				{
					MINT_DELETE(_sharedRefCounter);
				}
			}

			_sharedRefCounter = rhs._sharedRefCounter;
			_rawPtr = rhs._rawPtr;

			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->IncreaseWeakRefCount();
			}
			return *this;
		}
		SharedPtrViewer& operator=(const SharedPtrViewer& rhs) noexcept
		{
			if (this == &rhs)
			{
				return *this;
			}

			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->DecreaseWeakRefCount();

				if (_sharedRefCounter->GetStrongRefCount() == 0 && _sharedRefCounter->GetWeakRefCount() == 0)
				{
					MINT_DELETE(_sharedRefCounter);
				}
			}

			_sharedRefCounter = rhs._sharedRefCounter;
			_rawPtr = rhs._rawPtr;

			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->IncreaseWeakRefCount();
			}
			return *this;
		}
		SharedPtrViewer& operator=(SharedPtrViewer&& rhs) noexcept
		{
			if (this == &rhs)
			{
				return *this;
			}

			if (_sharedRefCounter != nullptr)
			{
				_sharedRefCounter->DecreaseWeakRefCount();

				if (_sharedRefCounter->GetStrongRefCount() == 0 && _sharedRefCounter->GetWeakRefCount() == 0)
				{
					MINT_DELETE(_sharedRefCounter);
				}
			}

			_sharedRefCounter = rhs._sharedRefCounter;
			_rawPtr = rhs._rawPtr;

			rhs._sharedRefCounter = nullptr;
			rhs._rawPtr = nullptr;
			return *this;
		}

	public:
		// this is really dangerous ...
		//T* operator&() noexcept { return _rawPtr; }
		T& operator*() noexcept { return *_rawPtr; }
		const T& operator*() const noexcept { return *_rawPtr; }
		T* operator->() noexcept { return _rawPtr; }
		const T* operator->() const noexcept { return _rawPtr; }

	public:
		bool IsValid() const { return (_sharedRefCounter == nullptr ? false : _sharedRefCounter->GetStrongRefCount() != 0); }

	private:
		SharedRefCounter* _sharedRefCounter;
		T* _rawPtr;
	};


	template<typename T>
	static SharedPtr<T> MakeShared()
	{
		return SharedPtr<T>(MINT_NEW(T));
	}

	template<typename T>
	static SharedPtr<T> MakeShared(const T& rhs)
	{
		return SharedPtr<T>(MINT_NEW(T, rhs));
	}

	template<typename T, typename U>
	static SharedPtr<T> MakeShared(const U& rhs)
	{
		return SharedPtr<T>(dynamic_cast<T*>(MINT_NEW(U, rhs)));
	}

	template<typename T>
	static SharedPtr<T> MakeShared(T&& rhs)
	{
		return SharedPtr<T>(MINT_NEW(T, rhs));
	}
}


#endif // !_MINT_CONTAINER_SHARED_PTR_H_
