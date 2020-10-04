#pragma once


#ifndef FS_UNIQUE_STRING_H
#define FS_UNIQUE_STRING_H


#include <CommonDefinitions.h>


namespace fs
{
	class UniqueStringPoolA;


	class UniqueStringA
	{
		friend UniqueStringPoolA;

	public:
		static constexpr uint32						kInvalidIndex = kUint32Max;

	private:
													UniqueStringA(const UniqueStringPoolA* const pool, const uint32 index);
	
	public:
													UniqueStringA(const UniqueStringA& rhs) = default;
													UniqueStringA(UniqueStringA&& rhs) noexcept = default;
													~UniqueStringA() = default;

	public:
		UniqueStringA&								operator=(const UniqueStringA& rhs) = default;
		UniqueStringA&								operator=(UniqueStringA&& rhs) noexcept = default;

	public:
		const bool									operator==(const UniqueStringA& rhs) const noexcept;
		const bool									operator!=(const UniqueStringA& rhs) const noexcept;

	public:
		const char*									c_str() const noexcept;

	private:
		const UniqueStringPoolA*					_pool;
		uint32										_index;

#if defined FS_DEBUG
	private:
		const char*									_str{};
#endif

	public:
		static const UniqueStringA					kInvalidUniqueString;
	};
	

	class UniqueStringPoolA final
	{
		static constexpr uint32						kDefaultRawCapacity = 1024;

	public:
		UniqueStringPoolA();
		~UniqueStringPoolA();

	public:
		const UniqueStringA							registerString(const char* const rawString) noexcept;
		const UniqueStringA							getString(const uint32 uniqueStringIndex) const noexcept;
		const char*									getRawString(const UniqueStringA& uniqueString) const noexcept;

	public:
		void										reserve(const uint32 rawCapacity);

	private:
		std::mutex									_mutex;

	private:
		std::vector<uint32>							_offsetArray;
		char*										_rawMemory;
		uint32										_rawCapacity;
		uint32										_totalLength;
		uint32										_count;
	};
}


#endif // !FS_UNIQUE_STRING_H
