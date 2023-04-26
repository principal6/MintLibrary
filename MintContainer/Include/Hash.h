#pragma once


#ifndef _MINT_CONTAINER_HASH_H_
#define _MINT_CONTAINER_HASH_H_


#include <type_traits>
#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>


namespace mint
{
	template <typename T, typename = void>
	class HasMethodComputeHash : public std::false_type {};

	template <typename T>
	class HasMethodComputeHash<T, std::enable_if_t<std::is_member_function_pointer<decltype(&T::ComputeHash)>::value, void>> : public std::true_type {};

	uint64 ComputeHash(const char* const rhs, const uint32 length) noexcept;
	uint64 ComputeHash(const char* const rhs) noexcept;
	uint64 ComputeHash(const wchar_t* const rhs) noexcept;

	template <typename T>
	uint64 ComputeHash(const T& value) noexcept;


	template <typename T>
	struct Hasher final
	{
		uint64 operator()(const T& value) const noexcept;
	};

	template <>
	struct Hasher<std::string> final
	{
		uint64 operator()(const std::string& value) const noexcept;
	};
}


#endif // !_MINT_CONTAINER_HASH_H_
