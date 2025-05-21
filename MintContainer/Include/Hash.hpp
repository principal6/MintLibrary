#pragma once


#include <MintContainer/Include/Hash.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StringUtil.h>


namespace mint
{
	MINT_INLINE uint64 ComputeHash(const char* const rhs, const uint32 length) noexcept
	{
		// Hashing algorithm: FNV1a

		if (StringUtil::IsNullOrEmpty(rhs) == true)
		{
			return kUint64Max;
		}

		static constexpr uint64 kOffset = 0xcbf29ce484222325;
		static constexpr uint64 kPrime = 0x00000100000001B3;

		uint64 hash = kOffset;
		for (uint32 rawStringAt = 0; rawStringAt < length; ++rawStringAt)
		{
			hash ^= static_cast<uint8>(rhs[rawStringAt]);
			hash *= kPrime;
		}
		return hash;
	}

	MINT_INLINE uint64 ComputeHash(const char* const rhs) noexcept
	{
		const uint32 rhsLength = StringUtil::Length(rhs);
		return ComputeHash(rhs, rhsLength);
	}

	MINT_INLINE uint64 ComputeHash(const wchar_t* const rhs) noexcept
	{
		const uint32 rhsLength = StringUtil::Length(rhs);
		const char* const rhsA = reinterpret_cast<const char*>(rhs);
		return ComputeHash(rhsA, rhsLength * 2);
	}

	MINT_INLINE uint64 ComputeHash(const char8_t* const rhs) noexcept
	{
		const uint32 rhsLength = StringUtil::Length(rhs);
		const char* const rhsA = reinterpret_cast<const char*>(rhs);
		return ComputeHash(rhsA, rhsLength);
	}

	template <typename T>
	uint64 ComputeHash(const T& value) noexcept
	{
		if constexpr (std::is_arithmetic<T>::value == true)
		{
			const char* const str = reinterpret_cast<const char*>(&value);
			const uint32 length = sizeof(value);
			return ComputeHash(str, length);
		}
		else if constexpr (std::is_pointer<T>::value == true)
		{
			const char* const str = reinterpret_cast<const char*>(&value);
			const uint32 length = sizeof(value);
			return ComputeHash(str, length);
		}

		MINT_ASSERT(false, "Hash computation not implemented for this type!");
		return 0;
	}


	template<typename T, typename Enable>
	inline uint64 Hasher<T, Enable>::operator()(const T& value) const noexcept
	{
		if constexpr (HasMethodComputeHash<T>::value == true)
		{
			return value.ComputeHash();
		}
		else
		{
			return ComputeHash<T>(value);
		}
	}

	inline uint64 Hasher<std::string>::operator()(const std::string& value) const noexcept
	{
		return ComputeHash(value.c_str(), static_cast<uint32>(value.length()));
	}
}
