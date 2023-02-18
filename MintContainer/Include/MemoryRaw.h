#pragma once


#ifndef _MINT_CONTAINER_MEMORY_RAW_H_
#define _MINT_CONTAINER_MEMORY_RAW_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	namespace MemoryRaw
	{
#pragma region Type Traits
		template<typename T>
		constexpr bool IsConstructible() noexcept;

		template<typename T>
		constexpr bool IsDefaultConstructible() noexcept;

		template<typename T>
		constexpr bool IsCopyConstructible() noexcept;

		template<typename T>
		constexpr bool IsCopyAssignable() noexcept;

		template<typename T>
		constexpr bool IsCopiable() noexcept;

		template<typename T>
		constexpr bool IsMoveConstructible() noexcept;

		template<typename T>
		constexpr bool IsMoveAssignable() noexcept;

		template<typename T>
		constexpr bool IsMovable() noexcept;
#pragma endregion


		template<typename T>
		T* AllocateMemory(const uint32 size) noexcept;

		template<typename T>
		void MoveMemory_(T* const to, const T* const from, const uint32 count) noexcept;

		template<typename T>
		void DeallocateMemory(T*& rawPointer) noexcept;


		template<typename T>
		void Construct(T& at) noexcept;

		template<typename T>
		void CopyAssign(T& to, const T& from) noexcept;

		template<typename T>
		void MoveAssign(T& to, T&& from) noexcept;

		template<typename T>
		void CopyConstruct(T& to, const T& from) noexcept;

		template<typename T>
		void MoveConstruct(T& to, T&& from) noexcept;

		template<typename T>
		void Destroy(T& at) noexcept;
	}
}


#endif // !_MINT_CONTAINER_MEMORY_RAW_H_
