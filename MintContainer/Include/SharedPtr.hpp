#pragma once


#ifndef _MINT_CONTAINER_SHARED_PTR_HPP_
#define _MINT_CONTAINER_SHARED_PTR_HPP_


#include <MintContainer/Include/SharedPtr.h>


namespace mint
{
	// In order to allow the forward declaration of T when using SharedPtr class,
	// we must delete _rawPtr in a separate file, which is not the header file.
	template<typename T>
	void SharedPtr<T>::DecreaseReferenceCount()
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
}


#endif // !_MINT_CONTAINER_SHARED_PTR_HPP_
