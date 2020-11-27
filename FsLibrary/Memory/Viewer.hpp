#pragma once


#include <stdafx.h>

#include <Memory/Viewer.h>
#include <Memory/Allocator.hpp>
#include <Memory/Owner.hpp>


namespace fs
{
	namespace Memory
	{
		template<typename T>
		inline const T& ScopedViewer<T>::operator*() const noexcept
		{
			return viewData();
		}

		template<typename T>
		inline const T& ScopedViewer<T>::viewData() const noexcept
		{
			return *_memoryAccesor.getMemory();
		}


		template<typename T>
		inline Viewer<T>::Viewer()
			: _owner{ nullptr }
		{
			__noop;
		}

		template<typename T>
		inline Viewer<T>::Viewer(const Owner<T>& rhs)
		{
			_owner = &rhs;
		}

		template<typename T>
		inline Viewer<T>& Viewer<T>::operator=(const Owner<T>& rhs)
		{
			_owner = &rhs;

			return *this;
		}

		template<typename T>
		inline const bool Viewer<T>::isAlive() const noexcept
		{
			return (_owner != nullptr) ? _owner->isValid() : false;
		}

		template<typename T>
		inline ScopedViewer<T> Viewer<T>::viewDataSafe() const noexcept
		{
			FS_ASSERT("김장원", isAlive() == true, "getScopedViewer() 이전에 isAlive() 로 확인하세요!!!");
			
			return ScopedViewer(_owner->_memoryAccesor);
		}

		template<typename T>
		inline const T& Viewer<T>::viewData() const noexcept
		{
			FS_ASSERT("김장원", isAlive() == true, "viewDataUnsafe() 이전에 isAlive() 로 확인하세요!!!");

			return *(_owner->_memoryAccesor.getMemory());
		}

	}
}
