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

		// malloc 을 이용하여 ctor 를 호출하지 않고 메모리만 할당한다.
		// - 반드시 이 함수 호출 후에 명시적으로 ctor 를 호출해야 한다!
		template<typename T>
		T* AllocateMemory(const uint32 size) noexcept;

		// memory 를 move 한다. (to 와 from 의 메모리가 겹치더라도 안전하게 옮긴다)
		// move ctor, copy ctor 또는 move assignment, copy assignment 가 호출되지 않는다!!!
		template<typename T>
		void MoveMemory_(T* const to, const T* const from, const uint32 count) noexcept;

		// free 를 이용하여 dtor 를 호출하지 않고 메모리만 해제한다.
		// - 반드시 이 함수 호출 전에 명시적으로 dtor 를 호출했어야 한다!
		template<typename T>
		void DeallocateMemory(T*& rawPointer) noexcept;


		// placement new 를 사용하여 at 에 대해 default ctor 를 호출한다.
		// - at 이 살아있는 instance 라면 이 함수 호출 전에 반드시 dtor 를 명시적으로 호출했어야 한다!
		template<typename T>
		void Construct(T& at) noexcept;

		// to 가 살아있는 instance 여야만 한다!
		template<typename T>
		void CopyAssign(T& to, const T& from) noexcept;

		// to 가 살아있는 instance 여야만 한다!
		template<typename T>
		void MoveAssign(T& to, T&& from) noexcept;

		// placement new 를 사용하여 to 에 대해 copy ctor 를 호출한다.
		// - ctor 가 반드시 호출되는 것을 보장한다.
		// - to 가 살아있는 instance 라면 이 함수 호출 전에 반드시 dtor 를 명시적으로 호출했어야 한다!
		template<typename T>
		void CopyConstruct(T& to, const T& from) noexcept;

		// placement new 를 사용하여 to 에 대해 move ctor 를 호출한다.
		// - ctor 가 반드시 호출되는 것을 보장한다.
		// - to 가 살아있는 instance 라면 이 함수 호출 전에 반드시 dtor 를 명시적으로 호출했어야 한다!
		template<typename T>
		void MoveConstruct(T& to, T&& from) noexcept;

		// at 에 대해 dtor 를 호출한다.
		// - 반드시 placement new 로 생성된 instance 에 대해서만 사용되어야 한다.
		// - 반드시 at 이 살아있는 instance 여야만 한다.
		template<typename T>
		void Destroy(T& at) noexcept;
	}
}


#endif // !_MINT_CONTAINER_MEMORY_RAW_H_
