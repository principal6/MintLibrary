#include <MintECS/Include/TestECS.h>
#include <MintECS/Include/AllHpps.h>


namespace mint
{
	namespace TestECS
	{
		extern ECS::EntityComponentPool<EntityType0, TestComponent>* gInstance0;
		extern ECS::EntityComponentPool<EntityType0, TestComponent>* gInstance1;

		void TestStaticInstance()
		{
			using namespace ECS;

			gInstance1 = &EntityComponentPool<EntityType0, TestComponent>::GetInstance();
		}
	}
}
