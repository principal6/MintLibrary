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


	class ReferenceCounter
	{
	public:
		ReferenceCounter() : _strongReferenceCount{ 0 }, _weakReferenceCount{ 0 } { __noop; }
		ReferenceCounter(const ReferenceCounter& rhs) = delete;
		ReferenceCounter(ReferenceCounter&& rhs) noexcept
			: _strongReferenceCount{ rhs._strongReferenceCount }
			, _weakReferenceCount{ rhs._weakReferenceCount }
		{
			rhs._strongReferenceCount = 0;
			rhs._weakReferenceCount = 0;
		}
		~ReferenceCounter() { __noop; }
		ReferenceCounter& operator=(const ReferenceCounter& rhs) = delete;
		ReferenceCounter& operator=(ReferenceCounter&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_strongReferenceCount = rhs._strongReferenceCount;
				_weakReferenceCount = rhs._weakReferenceCount;

				rhs._strongReferenceCount = 0;
				rhs._weakReferenceCount = 0;
			}
			return *this;
		}
	public:
		MINT_INLINE void IncreaseStrongReferenceCount() { ++_strongReferenceCount; }
		MINT_INLINE void IncreaseWeakReferenceCount() { ++_weakReferenceCount; }
		MINT_INLINE void DecreaseStrongReferenceCount() { --_strongReferenceCount; }
		MINT_INLINE void DecreaseWeakReferenceCount() { --_weakReferenceCount; }
		MINT_INLINE int32 GetStrongReferenceCount() const { return _strongReferenceCount; }
		MINT_INLINE int32 GetWeakReferenceCount() const { return _weakReferenceCount; }
	private:
		int32 _strongReferenceCount;
		int32 _weakReferenceCount;
	};


	template<typename T>
	class SharedPtr
	{
		friend SharedPtrViewer;

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
			: _referenceCounter{ rhs._referenceCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			if (_referenceCounter != nullptr)
			{
				_referenceCounter->IncreaseStrongReferenceCount();
			}
		}
		SharedPtr(SharedPtr&& rhs)
			: _referenceCounter{ rhs._referenceCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			rhs._referenceCounter = nullptr;
			rhs._rawPtr = nullptr;
		}
		~SharedPtr()
		{
			if (_referenceCounter != nullptr)
			{
				_referenceCounter->DecreaseStrongReferenceCount();

				if (_referenceCounter->GetStrongReferenceCount() == 0)
				{
					MINT_DELETE(_rawPtr);

					if (_referenceCounter->GetWeakReferenceCount() == 0)
					{
						MINT_DELETE(_referenceCounter);
					}
				}
			}
		}
		SharedPtr& operator=(const SharedPtr& rhs) noexcept
		{
			if (this == &rhs)
			{
				return *this;
			}

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->DecreaseStrongReferenceCount();

				if (_referenceCounter->GetStrongReferenceCount() == 0)
				{
					MINT_DELETE(_rawPtr);

					if (_referenceCounter->GetWeakReferenceCount() == 0)
					{
						MINT_DELETE(_referenceCounter);
					}
				}
			}

			_referenceCounter = rhs._referenceCounter;
			_rawPtr = rhs._rawPtr;

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->IncreaseStrongReferenceCount();
			}
			return *this;
		}
		SharedPtr& operator=(SharedPtr&& rhs) noexcept
		{
			if (this == &rhs)
			{
				return *this;
			}

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->DecreaseStrongReferenceCount();

				if (_referenceCounter->GetStrongReferenceCount() == 0)
				{
					MINT_DELETE(_rawPtr);

					if (_referenceCounter->GetWeakReferenceCount() == 0)
					{
						MINT_DELETE(_referenceCounter);
					}
				}
			}

			_referenceCounter = rhs._referenceCounter;
			_rawPtr = rhs._rawPtr;

			rhs._referenceCounter = nullptr;
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
		bool IsValid() const { return (_referenceCounter == nullptr ? false : _referenceCounter->GetStrongReferenceCount() != 0); }

	private:
		SharedPtr(T* const rawPointer)
			: _referenceCounter{ MINT_NEW(ReferenceCounter) }
			, _rawPtr{ rawPointer }
		{
			_referenceCounter->IncreaseStrongReferenceCount();
		}

	private:
		ReferenceCounter* _referenceCounter = nullptr;
		T* _rawPtr = nullptr;
	};

	template<typename T>
	class SharedPtrViewer
	{
	public:
		SharedPtrViewer() = default;
		SharedPtrViewer(const SharedPtr<T>& rhs)
			: _referenceCounter{ rhs._referenceCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			if (_referenceCounter != nullptr)
			{
				_referenceCounter->IncreaseWeakReferenceCount();
			}
		}
		SharedPtrViewer(const SharedPtrViewer& rhs)
			: _referenceCounter{ rhs._referenceCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			if (_referenceCounter != nullptr)
			{
				_referenceCounter->IncreaseWeakReferenceCount();
			}
		}
		SharedPtrViewer(SharedPtrViewer&& rhs)
			: _referenceCounter{ rhs._referenceCounter }
			, _rawPtr{ rhs._rawPtr }
		{
			rhs._referenceCounter = nullptr;
			rhs._rawPtr = nullptr;
		}
		~SharedPtrViewer()
		{
			if (_referenceCounter != nullptr)
			{
				_referenceCounter->DecreaseWeakReferenceCount();

				if (_referenceCounter->GetStrongReferenceCount() == 0 && _referenceCounter->GetWeakReferenceCount() == 0)
				{
					MINT_DELETE(_referenceCounter);
				}
			}
		}
		SharedPtrViewer& operator=(const SharedPtr<T>& rhs) noexcept
		{
			if (_referenceCounter == rhs._referenceCounter)
			{
				return *this;
			}

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->DecreaseWeakReferenceCount();

				if (_referenceCounter->GetStrongReferenceCount() == 0 && _referenceCounter->GetWeakReferenceCount() == 0)
				{
					MINT_DELETE(_referenceCounter);
				}
			}

			_referenceCounter = rhs._referenceCounter;
			_rawPtr = rhs._rawPtr;

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->IncreaseWeakReferenceCount();
			}
			return *this;
		}
		SharedPtrViewer& operator=(const SharedPtrViewer& rhs) noexcept
		{
			if (this == &rhs)
			{
				return *this;
			}

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->DecreaseWeakReferenceCount();

				if (_referenceCounter->GetStrongReferenceCount() == 0 && _referenceCounter->GetWeakReferenceCount() == 0)
				{
					MINT_DELETE(_referenceCounter);
				}
			}

			_referenceCounter = rhs._referenceCounter;
			_rawPtr = rhs._rawPtr;

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->IncreaseWeakReferenceCount();
			}
			return *this;
		}
		SharedPtrViewer& operator=(SharedPtrViewer&& rhs) noexcept
		{
			if (this == &rhs)
			{
				return *this;
			}

			if (_referenceCounter != nullptr)
			{
				_referenceCounter->DecreaseWeakReferenceCount();

				if (_referenceCounter->GetStrongReferenceCount() == 0 && _referenceCounter->GetWeakReferenceCount() == 0)
				{
					MINT_DELETE(_referenceCounter);
				}
			}

			_referenceCounter = rhs._referenceCounter;
			_rawPtr = rhs._rawPtr;

			rhs._referenceCounter = nullptr;
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
		bool IsValid() const { return (_referenceCounter == nullptr ? false : _referenceCounter->GetStrongReferenceCount() != 0); }

	private:
		ReferenceCounter* _referenceCounter;
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
