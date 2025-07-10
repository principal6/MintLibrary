#pragma once


#include <MintECS/Include/AllHeaders.h>


namespace mint
{
	namespace TestECS
	{
		class EntityType0 : public ECS::EntityBase<uint64, 40> {};
		class EntityType1 : public ECS::EntityBase<uint64, 40> {};

		struct TestComponent
		{
		};

		void Test();
	}
}
