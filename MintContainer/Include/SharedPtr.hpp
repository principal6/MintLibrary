#pragma once


#ifndef _MINT_CONTAINER_SHARED_PTR_HPP_
#define _MINT_CONTAINER_SHARED_PTR_HPP_


#include <MintContainer/Include/SharedPtr.h>


namespace mint
{
	// In order to allow the forward declaration of T when using SharedPtr class,
	// we must delete _rawPtr in a separate file, which is not the header file.
	template<typename T>
	void SharedPtr<T>::DecreaseRefCount()
	{
		if (_sharedRefCounter != nullptr)
		{
			_sharedRefCounter->DecreaseStrongRefCount();

			if (_sharedRefCounter->GetStrongRefCount() == 0)
			{
				MINT_DELETE(_rawPtr);

				if (_sharedRefCounter->GetWeakRefCount() == 0)
				{
					MINT_DELETE(_sharedRefCounter);
				}
			}
		}
	}
}


#endif // !_MINT_CONTAINER_SHARED_PTR_HPP_
