#pragma once


#ifndef _MINT_CONTAINER_REFCOUNTED_H_
#define _MINT_CONTAINER_REFCOUNTED_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	// Unlike OwnPtr or SharedPtr, this class allows manual reference counting.
	template<typename T>
	class RefCounted
	{
	public:
		RefCounted() : RefCounted(nullptr) { __noop; }
		explicit RefCounted(T* resource)
			: _resource{ resource }
			// _refCount must be 0 here in case of when the user doesn't call any IncreaseRefCount() and DecreaseRefCount() until destruction.
			, _refCount{ 0 }
		{
			__noop;
		}
		RefCounted(const RefCounted& rhs) = delete;
		RefCounted(RefCounted&& rhs)
			: _resource{ rhs._resource }
			, _refCount{ rhs._refCount }
		{
			rhs._resource = nullptr;
			rhs._refCount = 0;
		}
		~RefCounted();

	public:
		RefCounted& operator=(const RefCounted& rhs) = delete;
		RefCounted& operator=(RefCounted&& rhs)
		{
			if (this != &rhs)
			{
				MINT_ASSERT(_refCount == 0 && _resource == nullptr, "This must not be a valid RefCounted object!");

				_resource = rhs._resource;
				_refCount = rhs._refCount;

				rhs._resource = nullptr;
				rhs._refCount = 0;
			}
			return *this;
		}

	public:
		MINT_INLINE void IncreaseRefCount() { ++_refCount; }
		void DecreaseRefCount();
		MINT_INLINE bool IsValid() const noexcept { return _resource != nullptr; }
		MINT_INLINE int32 GetRefCount() const noexcept { return _refCount; }
		MINT_INLINE T* Get() const noexcept { return _resource; }
		MINT_INLINE T* operator->() const noexcept { return _resource; }
		MINT_INLINE T& operator*() const noexcept { return *_resource; }

	private:
		T* _resource = nullptr;
		int32 _refCount;
	};
}


#endif // !_MINT_CONTAINER_REFCOUNTED_H_
