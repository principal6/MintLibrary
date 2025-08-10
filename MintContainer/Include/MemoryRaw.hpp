#pragma once


#ifndef _MINT_CONTAINER_MEMORY_RAW_HPP_
#define _MINT_CONTAINER_MEMORY_RAW_HPP_


#include <MintContainer/Include/MemoryRaw.h>

#include <type_traits>


namespace mint
{
#pragma region Type Traitsㅊ
	template<typename T>
	MINT_INLINE constexpr bool IsConstructible() noexcept
	{
		return (IsDefaultConstructible<T>() || IsCopyConstructible<T>() || IsMoveConstructible<T>());
	}

	template<typename T>
	MINT_INLINE constexpr bool IsDefaultConstructible() noexcept
	{
		return (std::is_default_constructible<T>::value == true);
	}

	template<typename T>
	MINT_INLINE constexpr bool IsCopyConstructible() noexcept
	{
		return (std::is_copy_constructible<T>::value == true);
	}

	template<typename T>
	MINT_INLINE constexpr bool IsCopyAssignable() noexcept
	{
		return (std::is_copy_assignable<T>::value == true);
	}

	template<typename T>
	MINT_INLINE constexpr bool IsCopiable() noexcept
	{
		return (IsCopyConstructible<T>() || IsCopyAssignable<T>());
	}

	template<typename T>
	MINT_INLINE constexpr bool IsMoveConstructible() noexcept
	{
		return (std::is_move_constructible<T>::value == true);
	}

	template<typename T>
	MINT_INLINE constexpr bool IsMoveAssignable() noexcept
	{
		return (std::is_move_assignable<T>::value == true);
	}

	template<typename T>
	MINT_INLINE constexpr bool IsMovable() noexcept
	{
		return (IsMoveConstructible<T>() || IsMoveAssignable<T>());
	}
#pragma endregion

	namespace MemoryRaw
	{
		template<typename T>
		MINT_INLINE T* AllocateMemory(const uint32 size) noexcept
		{
			static_assert(IsConstructible<T>(), "T is not constructible type!!!");

			return MINT_MALLOC(T, size);
		}

		template<typename T>
		MINT_INLINE void MoveMemory_(T* const to, const T* const from, const uint32 count) noexcept
		{
			std::memmove(to, from, sizeof(T) * count);
		}

		template<typename T>
		MINT_INLINE void DeallocateMemory(T*& rawPointer) noexcept
		{
			if (rawPointer == nullptr)
			{
				return;
			}

			MINT_FREE(rawPointer);
		}

		template<typename T>
		MINT_INLINE void ConstructAt(T& at) noexcept
		{
			static_assert(IsDefaultConstructible<T>(), "T is not default-constructible!!!");

			MINT_PLACEMNT_NEW(&at, T());
		}

		template<typename T>
		// to 가 살아있는 instance 라면 반드시 dtor 를 명시적으로 호출해야 한다!
		MINT_INLINE void CopyConstructRaw(T& to, const T& from) noexcept
		{
			MINT_PLACEMNT_NEW(&to, T(from));
		}

		template<typename T>
		// to 가 살아있는 instance 라면 반드시 dtor 를 명시적으로 호출해야 한다!
		MINT_INLINE void MoveConstructRaw(T& to, T&& from) noexcept
		{
			MINT_PLACEMNT_NEW(&to, T(std::move(from)));
		}

		template<typename T>
		MINT_INLINE void CopyConstructAt(T& to, const T& from) noexcept
		{
			static_assert(IsCopyConstructible<T>() || (IsDefaultConstructible<T>() == true && IsCopyAssignable<T>() == true), "T is not copiable!!!");

			if constexpr (IsCopyConstructible<T>() == true)
			{
				CopyConstructRaw<T>(to, from);
			}
			else if constexpr (IsDefaultConstructible<T>() == true && IsCopyAssignable<T>() == true)
			{
				ConstructAt<T>(to);

				to = from;
			}
		}

		template<typename T>
		MINT_INLINE void MoveConstructAt(T& to, T&& from) noexcept
		{
			static_assert(IsMoveConstructible<T>() || (IsDefaultConstructible<T>() == true && IsMoveAssignable<T>() == true), "T is not movable!!!");

			if constexpr (IsMoveConstructible<T>() == true)
			{
				MoveConstructRaw<T>(to, std::move(from));
			}
			else if constexpr (IsDefaultConstructible<T>() == true && IsMoveAssignable<T>() == true)
			{
				ConstructAt<T>(to);

				to = std::move(from);
			}
		}

		template<typename T>
		MINT_INLINE void DestroyAt(T& at) noexcept
		{
			at.~T();
		}
	}
}


#endif // !_MINT_CONTAINER_MEMORY_RAW_HPP_
