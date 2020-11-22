#pragma once


#ifndef FS_REF_PTR_H
#define FS_REF_PTR_H


#include <CommonDefinitions.h>

#include <Memory\Allocator.h>


namespace fs
{
	namespace Memory
	{
		template <typename T>
		class Viewer
		{
		public:
												Viewer();
												Viewer(const T& instance);
												Viewer(T&& instance) = delete;
												Viewer(const Viewer& rhs);
												Viewer(Viewer&& rhs) noexcept;
												
												~Viewer() = default;

		public:
			Viewer&								operator=(const T& rhs);
			Viewer&								operator=(T&& rhs) noexcept = delete;

			Viewer&								operator=(const Viewer& rhs);
			Viewer&								operator=(Viewer&& rhs) noexcept;

		public:
			const T*							viewData() const noexcept;
			const bool							isAlive() const noexcept;

		private:
			fs::Memory::Allocator<T>* const		_memoryAllocator;
			fs::Memory::Accessor<T>				_memoryAccesor;
		};
	}
}


#endif // !FS_REF_PTR_H