#pragma once


#ifndef FS_VIEWER_H
#define FS_VIEWER_H


#include <FsLibrary/CommonDefinitions.h>

#include <FsLibrary/Memory/Allocator.h>


namespace fs
{
	namespace Memory
	{
		template <typename T>
		class Owner;


		template <typename T>
		class Viewer;


		template <typename T>
		class ScopedViewer
		{
			friend Viewer;

		private:
												ScopedViewer(const fs::Memory::Accessor<T> memoryAccessor) : _memoryAccesor{ memoryAccessor } { __noop; }
												ScopedViewer(const ScopedViewer& rhs) = delete;
												ScopedViewer(ScopedViewer&& rhs) noexcept = delete;
		public:
												~ScopedViewer() = default;
		
		private:
			ScopedViewer&						operator=(const ScopedViewer& rhs) = delete;
			ScopedViewer&						operator=(ScopedViewer&& rhs) noexcept = delete;
		
		public:
			const T&							operator*() const noexcept;
			
		public:
			const T&							viewData() const noexcept;
		
		private:
			fs::Memory::Accessor<T>				_memoryAccesor;
		};


		// Viewer can delay deallocation of T
		template <typename T>
		class Viewer
		{
		public:
												Viewer();
												Viewer(const Owner<T>& owner);
												Viewer(const Viewer& rhs) = default;
												Viewer(Viewer&& rhs) noexcept = default;
												~Viewer() = default;

		public:
			Viewer&								operator=(const Owner<T>& owner);
			Viewer&								operator=(const Viewer& rhs) = default;
			Viewer&								operator=(Viewer&& rhs) noexcept = default;

		public:
			const bool							isAlive() const noexcept;
		
		public:
			ScopedViewer<T>						viewDataSafe() const noexcept;
			const T&							viewData() const noexcept;

		private:
			const Owner<T>*						_owner;
		};
	}
}


#endif // !FS_VIEWER_H