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
			: _resource(resource)
			// _refCount must be 0 here in case of when the user doesn't call any IncreaseReference() and DecreaseReference() until destruction.
			, _refCount(0)
		{
			__noop;
		}
		~RefCounted();

	public:
		MINT_INLINE void IncreaseReference() { ++_refCount; }
		void DecreaseReference();
		MINT_INLINE T* Get() const noexcept { return _resource; }
		MINT_INLINE T* operator->() const noexcept { return _resource; }
		MINT_INLINE T& operator*() const noexcept { return *_resource; }
		MINT_INLINE int32 GetRefCount() const noexcept { return _refCount; }

	private:
		T* _resource = nullptr;
		int32 _refCount;
	};
}


#endif // !_MINT_CONTAINER_REFCOUNTED_H_
