#include <MintECS/Include/TestECS.h>
#include <MintECS/Include/AllHpps.h>


namespace mint
{

	namespace TestECS
	{
		class TestBase {};
		class TestDerived : public TestBase {};

		template<typename T, typename Enable = void>
		struct TestEnable
		{
			uint64 operator()(const T& value) const noexcept { return 0; }
		};

		template<typename T> requires std::derived_from<T, TestBase>
		struct TestEnable<T>
		{
			uint64 operator()(const T& value) const noexcept { return 1; }
		};

		template<typename T> requires std::derived_from<T, ECS::EntityBase<typename T::UintType, T::kIndexBits>>
		struct TestEnable<T>
		{
			uint64 operator()(const T& value) const noexcept { return 2; }
		};

		void Test()
		{
			using namespace ECS;

			TestDerived testDerived;
			MINT_ASSERT(TestEnable<TestDerived>()(testDerived) == 1llu, "!!!");

			class EntityDerived : public ECS::EntityBase<uint64, 40> {};
			static_assert(std::is_base_of<ECS::EntityBase<uint64, 40>, EntityDerived>::value == true);
			EntityDerived testEntityDerived;
			MINT_ASSERT(TestEnable<EntityDerived>()(testEntityDerived) == 2llu, "!!!");
		}
	}
}
