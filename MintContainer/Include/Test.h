#pragma once


#ifndef _MINT_CONTAINER_TEST_H_
#define _MINT_CONTAINER_TEST_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	namespace TestContainers
	{
		bool test();
		bool test_Array();
		bool test_BitArray();
		bool test_BitVector();
		bool test_StackHolder();
		bool test_Vector();
		bool test_HashMap();
		bool test_SharedPtr();
		bool test_StringTypes();
		bool test_StringUtil();
		bool test_Queue();
		bool test_Tree();
	};
}


#endif // !_MINT_CONTAINER_TEST_H_
