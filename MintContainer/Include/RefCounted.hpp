#pragma once


#ifndef _MINT_CONTAINER_REFCOUNTED_HPP_
#define _MINT_CONTAINER_REFCOUNTED_HPP_


#include <MintContainer/Include/RefCounted.h>


namespace mint
{
	template<typename T>
	MINT_INLINE RefCounted<T>::~RefCounted()
	{
		MINT_ASSERT(_refCount == 0, "RefCount must be decreased before the destruction!");
		// If the user creates RefCounted and does nothing until destruction, then the resource must be deleted here.
		MINT_DELETE(_resource);
	}

	template<typename T>
	MINT_INLINE void RefCounted<T>::DecreaseReference()
	{
		MINT_ASSERT(_refCount > 0, "RefCount must be greater than 0!");
		--_refCount;

		if (_refCount == 0)
		{
			MINT_DELETE(_resource);
		}
	}
}


#endif // !_MINT_CONTAINER_REFCOUNTED_HPP_
