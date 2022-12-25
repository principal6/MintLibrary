#pragma once


#ifndef _MINT_CONTAINER_TEST_H_
#define _MINT_CONTAINER_TEST_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	namespace TestContainers
	{
		bool test() noexcept;

		bool test_Array() noexcept;
		bool test_BitArray() noexcept;
		bool test_BitVector() noexcept;
		bool test_StackHolder() noexcept;
		bool test_Vector() noexcept;
		bool test_HashMap() noexcept;
		bool test_StringTypes() noexcept;
		bool test_StringUtil() noexcept;
		bool test_Queue() noexcept;
		bool test_Tree() noexcept;
	}
}


#endif // !_MINT_CONTAINER_TEST_H_
