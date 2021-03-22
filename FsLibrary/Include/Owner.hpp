#pragma once


#include <stdafx.h>

#include <FsLibrary/Include/Owner.h>
#include <FsLibrary/Include/Allocator.hpp>


namespace fs
{
	namespace Memory
	{
		template<typename T>
		inline Owner<T>::Owner()
			: _memoryAllocator{ fs::Memory::Allocator<T>::getInstance() }
			, _memoryAccesor{ &_memoryAllocator }
		{
			__noop;
		}

		template<typename T>
		inline Owner<T>::Owner(T&& instance)
			: Owner()
		{
			_memoryAccesor = _memoryAllocator.allocate();

			_memoryAccesor.setMemory(&instance);
		}

		template<typename T>
		inline Owner<T>::Owner(Owner&& rhs) noexcept
			: Owner()
		{
			_memoryAccesor = std::move(rhs._memoryAccesor);
		}

		template<typename T>
		inline Owner<T>& Owner<T>::operator=(Owner<T>&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_memoryAccesor = std::move(rhs._memoryAccesor);
			}
			return *this;
		}

		template<typename T>
		inline const bool Owner<T>::isValid() const noexcept
		{
			return _memoryAccesor.isValid();
		}
		
		template<typename T>
		inline T& Owner<T>::accessData() noexcept
		{
			FS_ASSERT("김장원", _memoryAccesor.isValid() == true, "accessData() 이전에 isValid() 로 확인하세요!!!");

			return *_memoryAccesor.getMemoryXXX();
		}

		template<typename T>
		inline const T& Owner<T>::viewData() noexcept
		{
			FS_ASSERT("김장원", _memoryAccesor.isValid() == true, "accessData() 이전에 isValid() 로 확인하세요!!!");

			return *_memoryAccesor.getMemory();
		}
	}
}
