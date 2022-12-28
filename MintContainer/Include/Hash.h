#pragma once


#ifndef _MINT_CONTAINER_HASH_H_
#define _MINT_CONTAINER_HASH_H_


#include <type_traits>
#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>


namespace mint
{
	uint64 computeHash(const char* const rhs, const uint32 length) noexcept;
	uint64 computeHash(const char* const rhs) noexcept;
	uint64 computeHash(const wchar_t* const rhs) noexcept;

	template <typename T>
	uint64 computeHash(const T& value) noexcept;



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

	template <typename T>
	struct Hasher<String<T>> final
	{
		uint64 operator()(const String<T>& value) const noexcept;
	};
}


#endif // !_MINT_CONTAINER_HASH_H_
