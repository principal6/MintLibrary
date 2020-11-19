#pragma once


#include <stdafx.h>

#include <Memory\Viewer.h>
#include <Memory\Allocator.hpp>


namespace fs
{
	namespace Memory
	{
		template <typename T>
		Allocator<T> Viewer<T>::_memoryAllocator;


		template<typename T>
		inline Viewer<T>::Viewer()
			: _memoryAccesor{ &_memoryAllocator }
		{
			__noop;
		}

		template<typename T>
		inline Viewer<T>::Viewer(const T& instance)
			: _memoryAccesor{ &_memoryAllocator }
		{
			_memoryAccesor = _memoryAllocator.allocate();
		
			// instance 멤버 중 memoryAccessor 가 있다면 그 Reference Count 가 증가합니다!!!
			_memoryAccesor.setMemory(&instance);
		}

		/*
		template<typename T>
		inline RefPtr<T>::RefPtr(T&& instance)
			: _memoryAccesor{ &_memoryAllocator }
		{
			_memoryAccesor = _memoryAllocator.allocate();

			// instance 멤버 중 memoryAccessor 가 있다면 그 Reference Count 증가를 피하기 위해 필수!!!
			memcpy(_memoryAccesor.getMemoryXXX(), &instance, sizeof(T));
		}
		*/

		template<typename T>
		inline Viewer<T>::Viewer(const Viewer& rhs)
			: _memoryAccesor{ &_memoryAllocator }
		{
			memcpy(&_memoryAccesor, &rhs._memoryAccesor, sizeof(_memoryAccesor));
		}

		template<typename T>
		inline Viewer<T>::Viewer(Viewer&& rhs) noexcept
			: _memoryAccesor{ std::move(rhs._memoryAccesor) }
		{
			__noop;
		}

		template<typename T>
		inline Viewer<T>& Viewer<T>::operator=(const T& rhs)
		{
			_memoryAccesor = _memoryAllocator.allocate();

			// instance 멤버 중 memoryAccessor 가 있다면 그 Reference Count 가 증가합니다!!!
			_memoryAccesor.setMemory(&rhs);

			return *this;
		}

		template<typename T>
		inline Viewer<T>& Viewer<T>::operator=(const Viewer<T>& rhs)
		{
			if (this != &rhs)
			{
				//_memoryAccesor = rhs._memoryAccesor;
				memcpy(&_memoryAccesor, &rhs._memoryAccesor, sizeof(_memoryAccesor));
			}
			return *this;
		}

		template<typename T>
		inline Viewer<T>& Viewer<T>::operator=(Viewer<T>&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_memoryAccesor = std::move(rhs._memoryAccesor);
			}
			return *this;
		}

		template<typename T>
		inline const T* Viewer<T>::viewData() const noexcept
		{
			return _memoryAccesor.getMemory();
		}

		template<typename T>
		inline const bool Viewer<T>::isAlive() const noexcept
		{
			return _memoryAccesor.isValid();
		}
	}
}
