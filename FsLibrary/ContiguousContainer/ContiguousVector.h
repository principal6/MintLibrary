#pragma once


#ifndef FS_VECTOR_H
#define FS_VECTOR_H


#include <CommonDefinitions.h>

#include <FsLibrary/Memory/Allocator.h>

#include <initializer_list>


namespace fs
{
	template <typename T>
	class ContiguousVector
	{
		static constexpr uint32				kDefaultCapacity = 8;

	private:
											ContiguousVector(fs::Memory::Allocator<T>& memoryAllocator, const uint32 capacity, const uint32 size);

	public:
											ContiguousVector();
											ContiguousVector(const std::initializer_list<T>& il);
		explicit							ContiguousVector(const uint32 capacity);
											ContiguousVector(const ContiguousVector& rhs);
											ContiguousVector(ContiguousVector&& rhs) noexcept;
											~ContiguousVector();

	public:
		ContiguousVector&								operator=(const ContiguousVector& rhs);
		ContiguousVector&								operator=(ContiguousVector&& rhs) noexcept;

	public:
		void								reserve(const uint32 capacity);
		void								resize(const uint32 size);

	public:
		void								clear();
		void								push_back(const T& value);
		void								push_back(T&& value);
		void								pop_back();
		void								insert(const uint32 at, const T& value);
		void								insert(const uint32 at, T&& value);
		void								erase(const uint32 at);

	public:
		void								set(const uint32 index, const T& value);
		void								set(const uint32 index, T&& value);
		const T&							get(const uint32 index) const;
		T&									get(const uint32 index);
		void								swap(const uint32 indexA, const uint32 indexB);

	public:
		const bool							empty() const noexcept;
		const bool							isFull() const noexcept;
		const uint32						capacity() const noexcept;
		const uint32						size() const noexcept;
		const T&							front() const;
		const T&							back() const;

	private:
		fs::Memory::Allocator<T>&			_memoryAllocator;
		fs::Memory::Accessor<T>				_memoryAccessor;
		uint32								_capacity;
		uint32								_size;
	};
}


#endif // !FS_VECTOR_H
