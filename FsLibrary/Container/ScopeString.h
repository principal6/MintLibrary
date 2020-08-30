#pragma once


#ifndef FS_SCOPE_STRING_H
#define FS_SCOPE_STRING_H


#include <CommonDefinitions.h>
#include <Container/StaticArray.h>


namespace fs
{
	template<uint32 BufferSize>
	class ScopeStringA
	{
	public:
											ScopeStringA();
											ScopeStringA(const char* const rawString);
											ScopeStringA(const ScopeStringA& rhs);
											ScopeStringA(ScopeStringA&& rhs) noexcept;
											~ScopeStringA();

	public:
		ScopeStringA&						operator=(const ScopeStringA& rhs) noexcept;
		ScopeStringA&						operator=(ScopeStringA&& rhs) noexcept;
		ScopeStringA&						operator=(const char* const rawString) noexcept;

	public:
		bool								operator==(const char* const rawString) const noexcept;
		bool								operator==(const ScopeStringA& rhs) const noexcept;
		bool								operator!=(const char* const rawString) const noexcept;
		bool								operator!=(const ScopeStringA& rhs) const noexcept;

	public:
		ScopeStringA&						operator+=(const char* const rawString) noexcept;
		ScopeStringA&						operator+=(const ScopeStringA& rhs) noexcept;

	public:
		char&								operator[](const uint32 at) noexcept;
		const char&							operator[](const uint32 at) const noexcept;

	public:
		uint32								capacity() const noexcept;
		uint32								length() const noexcept;
		const char*							c_str() const noexcept;

	public:
		char*								data() noexcept;

	private:
		bool								canInsert(const uint32 insertLength) const noexcept;

	public:
		ScopeStringA&						append(const char* const rawString) noexcept;
		ScopeStringA&						append(const ScopeStringA& rhs) noexcept;
		ScopeStringA&						assign(const char* const rawString) noexcept;
		ScopeStringA&						assign(const ScopeStringA& rhs) noexcept;

	public:
		ScopeStringA						substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
		uint32								find(const char* const rawString, const uint32 offset = kStringNPos) const noexcept;
		uint32								rfind(const char* const rawString, const uint32 offset = kStringNPos) const noexcept;
		bool								compare(const char* const rawString) const noexcept;
		bool								compare(const ScopeStringA& rhs) const noexcept;

	private:
		uint32								_length;
		StaticArray<char, BufferSize>		_raw;
	};
}


#endif // !FS_SCOPE_STRING_H
