#pragma once


#ifndef FS_DYNAMIC_STRING_H
#define FS_DYNAMIC_STRING_H


#include <CommonDefinitions.h>
#include <Memory\MemoryAllocator2.h>


namespace fs
{
	struct StringRange
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
		FS_INLINE const bool isLengthSet() const noexcept
		{
			return _length;
		}

	public:
		uint32	_offset;
		uint32	_length;
	};


	class DynamicStringA
	{
		static constexpr uint32				kMinCapacity{ 16 };
		
	public:
		template <class T>
		static const DynamicStringA			from_value(const typename std::enable_if<std::is_literal_type<T>::value, T>::type value) noexcept;

		static const bool					to_bool(const DynamicStringA& dynamicString) noexcept;
		static const int32					to_int32(const DynamicStringA& dynamicString) noexcept;
		static const uint32					to_uint32(const DynamicStringA& dynamicString) noexcept;
		static const float					to_float(const DynamicStringA& dynamicString) noexcept;
		static const double					to_double(const DynamicStringA& dynamicString) noexcept;

	public:
											DynamicStringA();
		explicit							DynamicStringA(const uint32 capacity);
											DynamicStringA(const char* const rawString);
		explicit							DynamicStringA(const char* const rawString, const uint32 rawStringLength);
											DynamicStringA(const DynamicStringA& rhs);
											DynamicStringA(DynamicStringA&& rhs) noexcept;
											~DynamicStringA();

	public:
		DynamicStringA&						operator=(const char* const rawString);
		DynamicStringA&						operator=(const DynamicStringA& rhs);
		DynamicStringA&						operator=(DynamicStringA&& rhs) noexcept;

	public:
		DynamicStringA&						operator+=(const char* const rawString) noexcept;
		DynamicStringA&						operator+=(const DynamicStringA& rhs) noexcept;

	public:
		const DynamicStringA				operator+(const char* const rawString) const noexcept;
		const DynamicStringA				operator+(const DynamicStringA& rhs) const noexcept;

	public:
		const bool							operator==(const char* const rawString) const noexcept;
		const bool							operator==(const DynamicStringA& rhs) const noexcept;
		const bool							operator!=(const char* const rawString) const noexcept;
		const bool							operator!=(const DynamicStringA& rhs) const noexcept;

	public:
		void								clear();
		void								assign(const char* const rawString);
		void								assign(const char* const rawString, const uint32 rawStringLength);
		void								assign(const DynamicStringA& rhs);
		void								assign(DynamicStringA&& rhs);
		void								append(const char* const rawString);
		void								append(const DynamicStringA& rhs);
		DynamicStringA						substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
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
		const bool							compare(const DynamicStringA& rhs) const noexcept;
		const bool							compare(const DynamicStringA& rhs, const StringRange& stringRange) const noexcept;

	private:
		const bool							compareInternal(const char* const rawString, const uint32 offset) const noexcept;
		const bool							compareInternal(const char* const rawString, const uint32 offset, const uint32 length) const noexcept;
		const bool							compareInternal(const DynamicStringA& rhs, const uint32 offset) const noexcept;
		const bool							compareInternal(const DynamicStringA& rhs, const uint32 offset, const uint32 length) const noexcept;

	public:
		const uint64						hash() const noexcept;

	private:
		void								setMemoryInternal(const char* const rawString, const uint32 offset = 0);
		
	private:
		static fs::Memory::Allocator<char>	_memoryAllocator;

	private:
		fs::Memory::Accessor<char>			_memoryAccessor;
		uint32								_length;
		mutable uint64						_cachedHash;
	};
}


#endif // !FS_DYNAMIC_STRING_H
