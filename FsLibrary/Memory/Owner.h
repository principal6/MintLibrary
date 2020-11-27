#pragma once


#ifndef FS_OWNER_H
#define FS_OWNER_H


#include <CommonDefinitions.h>

#include <Memory/Allocator.h>


namespace fs
{
	namespace Memory
	{
		template <typename T>
		class Viewer;


		// T maybe alive after destruction of Owner, because Viewer could be viewing T, which delay deallocation of it.
		// Owner is guaranteed to be unique. (No multiple Owners for the same instance of T)
		template <typename T>
		class Owner
		{
			friend Viewer;

		public:
												Owner();
												Owner(T&& instance);
												Owner(const Owner& rhs) = delete;
												Owner(Owner&& rhs) noexcept;
												~Owner() = default;

		public:
			Owner&								operator=(const Owner& rhs) = delete;
			Owner&								operator=(Owner&& rhs) noexcept;

		public:
			const bool							isValid() const noexcept;
			T&									accessData() noexcept;
			const T&							viewData() noexcept;

		private:
			fs::Memory::Allocator<T>&			_memoryAllocator;
			fs::Memory::Accessor<T>				_memoryAccesor;
		};
	}
}


#endif // !FS_OWNER_H