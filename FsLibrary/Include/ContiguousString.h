#pragma once


#ifndef FS_DYNAMIC_STRING_H
#define FS_DYNAMIC_STRING_H


#include <CommonDefinitions.h>

#include <FsLibrary/Include/Allocator.h>


namespace fs
{
	struct StringRange final
	{
								StringRange()
									: _offset{ 0 }
									, _length{ kUint32Max }
								{
									__noop;
								}

								StringRange(const uint32 offset)
									: _offset{ offset }
									, _length{ kUint32Max }
								{
									__noop;
								}

								StringRange(const uint32 offset, const uint32 length)
									: _offset{ offset }
									, _length{ length }
								{
									__noop;
								}

	public:
		FS_INLINE const bool	isLengthSet() const noexcept
		{
			return _length;
		}

	public:
		uint32					_offset;
		uint32					_length;
	};


	class ContiguousStringA
	{
		static constexpr uint32				kMinCapacity{ 16 };
		
	public:
		template <class T>
		static const ContiguousStringA			from_value(const typename std::enable_if<std::is_literal_type<T>::value, T>::type value) noexcept;

		static const bool					to_bool(const ContiguousStringA& dynamicString) noexcept;
		static const int32					to_int32(const ContiguousStringA& dynamicString) noexcept;
		static const uint32					to_uint32(const ContiguousStringA& dynamicString) noexcept;
		static const float					to_float(const ContiguousStringA& dynamicString) noexcept;
		static const double					to_double(const ContiguousStringA& dynamicString) noexcept;

	public:
											ContiguousStringA();
		explicit							ContiguousStringA(const uint32 capacity);
											ContiguousStringA(const char* const rawString);
		explicit							ContiguousStringA(const char* const rawString, const uint32 rawStringLength);
											ContiguousStringA(const ContiguousStringA& rhs);
											ContiguousStringA(ContiguousStringA&& rhs) noexcept;
											~ContiguousStringA();

	public:
		ContiguousStringA&						operator=(const char* const rawString);
		ContiguousStringA&						operator=(const ContiguousStringA& rhs);
		ContiguousStringA&						operator=(ContiguousStringA&& rhs) noexcept;

	public:
		ContiguousStringA&						operator+=(const char* const rawString) noexcept;
		ContiguousStringA&						operator+=(const ContiguousStringA& rhs) noexcept;

	public:
		const ContiguousStringA				operator+(const char* const rawString) const noexcept;
		const ContiguousStringA				operator+(const ContiguousStringA& rhs) const noexcept;

	public:
		const bool							operator==(const char* const rawString) const noexcept;
		const bool							operator==(const ContiguousStringA& rhs) const noexcept;
		const bool							operator!=(const char* const rawString) const noexcept;
		const bool							operator!=(const ContiguousStringA& rhs) const noexcept;

	public:
		void								clear();
		void								assign(const char* const rawString);
		void								assign(const char* const rawString, const uint32 rawStringLength);
		void								assign(const ContiguousStringA& rhs);
		void								assign(ContiguousStringA&& rhs);
		void								append(const char* const rawString);
		void								append(const ContiguousStringA& rhs);
		ContiguousStringA						substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
		ContiguousStringA						substr(const StringRange& stringRange) const noexcept;
		void								setChar(const uint32 at, const char ch);
		void								reserve(const uint32 newCapacity);
		void								resize(const uint32 newSize);
		void								push_back(const char ch);
		void								pop_back();

	public:
		const bool							empty() const noexcept;
		const uint32						length() const noexcept;
		const char* const					c_str() const noexcept;
		const char							getChar(const uint32 at) const noexcept;
		const char							front() const noexcept;
		const char							back() const noexcept;

	public:
		const uint32						find(const char* const rawString, const uint32 offset = 0) const noexcept;
		const uint32						rfind(const char* const rawString, const uint32 offset = 0) const noexcept;

	public:
		const bool							compare(const char* const rawString) const noexcept;
		const bool							compare(const char* const rawString, const StringRange& stringRange) const noexcept;
		const bool							compare(const ContiguousStringA& rhs) const noexcept;
		const bool							compare(const ContiguousStringA& rhs, const StringRange& stringRange) const noexcept;

	private:
		const bool							compareInternal(const char* const rawString, const uint32 offset) const noexcept;
		const bool							compareInternal(const char* const rawString, const uint32 offset, const uint32 length) const noexcept;
		const bool							compareInternal(const ContiguousStringA& rhs, const uint32 offset) const noexcept;
		const bool							compareInternal(const ContiguousStringA& rhs, const uint32 offset, const uint32 length) const noexcept;

	public:
		const uint64						hash() const noexcept;

	private:
		void								setMemoryInternal(const char* const rawString, const uint32 offset = 0);
		
	private:
		fs::Memory::Allocator<char>&		_memoryAllocator;

	private:
		fs::Memory::Accessor<char>			_memoryAccessor;
		uint32								_length;
		mutable uint64						_cachedHash;
	};
}


#endif // !FS_DYNAMIC_STRING_H
