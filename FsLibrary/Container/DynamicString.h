#pragma once


#ifndef FS_DYNAMIC_STRING_H
#define FS_DYNAMIC_STRING_H


#include <CommonDefinitions.h>
#include <Memory\MemoryAllocator2.h>


namespace fs
{
	class DynamicStringA
	{
		static constexpr uint32			kMinCapacity{ 16 };

	public:
										DynamicStringA();
		explicit						DynamicStringA(const uint32 capacity);
										DynamicStringA(const char* const rawString);
										DynamicStringA(const char* const rawString, const uint32 rawStringLength);
										DynamicStringA(const DynamicStringA& rhs);
										DynamicStringA(DynamicStringA&& rhs) noexcept;
										~DynamicStringA();

	public:
		DynamicStringA&					operator=(const char* const rawString);
		DynamicStringA&					operator=(const DynamicStringA& rhs);
		DynamicStringA&					operator=(DynamicStringA&& rhs) noexcept;

	public:
		DynamicStringA&					operator+=(const char* const rawString) noexcept;
		DynamicStringA&					operator+=(const DynamicStringA& rhs) noexcept;

	public:
		const DynamicStringA			operator+(const char* const rawString) const noexcept;
		const DynamicStringA			operator+(const DynamicStringA& rhs) const noexcept;

	public:
		const bool						operator==(const char* const rawString) const noexcept;
		const bool						operator==(const DynamicStringA& rhs) const noexcept;
		const bool						operator!=(const char* const rawString) const noexcept;
		const bool						operator!=(const DynamicStringA& rhs) const noexcept;

	public:
		void							clear();
		void							assign(const char* const rawString);
		void							assign(const char* const rawString, const uint32 rawStringLength);
		void							assign(const DynamicStringA& rhs);
		void							append(const char* const rawString);
		void							append(const DynamicStringA& rhs);
		DynamicStringA					substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
		void							setChar(const uint32 at, const char ch);
		void							reserve(const uint32 newCapacity);
		void							resize(const uint32 newSize);
		void							push_back(const char ch);
		void							pop_back();

		template <class T>
		static const DynamicStringA		from_value(const typename std::enable_if<std::is_literal_type<T>::value, T>::type value) noexcept;

	public:
		const bool						empty() const noexcept;
		const uint32					length() const noexcept;
		const char* const				c_str() const noexcept;
		const char						getChar(const uint32 at) const noexcept;
		const char						front() const noexcept;
		const char						back() const noexcept;

	public:
		const uint32					find(const char* const rawString, const uint32 offset = 0) const noexcept;
		const uint32					rfind(const char* const rawString, const uint32 offset = 0) const noexcept;
		const bool						compare(const char* const rawString) const noexcept;
		const bool						compare(const DynamicStringA& rhs) const noexcept;
		const uint64					hash() const noexcept;

	private:
		void							setMemoryInternal(const char* const rawString, const uint32 offset = 0);
		
	private:
		static MemoryAllocator2<char>	_memoryAllocator;

	private:
		MemoryAccessor2<char>			_memoryAccessor;
		uint32							_length;
		mutable uint64					_cachedHash;
	};
}


#endif // !FS_DYNAMIC_STRING_H
