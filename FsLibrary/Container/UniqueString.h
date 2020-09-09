#pragma once


#ifndef FS_UNIQUE_STRING_H
#define FS_UNIQUE_STRING_H


#include <CommonDefinitions.h>


namespace fs
{
	static constexpr uint32							kUniqueStringInvalidIndex = kUint32Max;

	template<uint32 Capacity>
	class UniqueStringAHolder
	{
	public:
													UniqueStringAHolder();
													~UniqueStringAHolder();

	public:
		static UniqueStringAHolder*					getInstance()
		{
			static UniqueStringAHolder instance;
			return &instance;
		}

	public:
		const uint32								registerString(const char* const rawString) noexcept;
		const char*									getString(const uint32 index) const noexcept;

	private:
		char										_raw[Capacity];
		uint32										_offsetArray[Capacity];
		uint32										_totalLength;
		uint32										_count;
	};


	template<uint32 HolderCapacity>
	class UniqueStringA
	{
	public:
													UniqueStringA();
													UniqueStringA(const char* const rawString);
													UniqueStringA(const UniqueStringA& rhs);
													UniqueStringA(UniqueStringA&& rhs) noexcept;
													~UniqueStringA();

	public:
		UniqueStringA&								operator=(const UniqueStringA& rhs);
		UniqueStringA&								operator=(UniqueStringA&& rhs) noexcept;

	public:
		const bool									operator==(const UniqueStringA& rhs);
		const bool									operator!=(const UniqueStringA& rhs);

	public:
		bool										isValid() const noexcept;
		bool										assign(const char* const rawString) noexcept;
		const char*									c_str() const noexcept;

#if defined FS_DEBUG
	private:
		void										setDebugString() noexcept;
#endif
	
	private:
		UniqueStringAHolder<HolderCapacity>* const	_holder;
		uint32										_index;

#if defined FS_DEBUG
	private:
		const char*									_str;
#endif
	};
	
}


#endif // !FS_UNIQUE_STRING_H
