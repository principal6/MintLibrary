#include <MintReflection/Include/Test.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/String.hpp>

#include <MintReflection/Include/Reflection.h>
#include <MintReflection/Include/Reflection.hpp>

#include <MintMath/Include/Float3.h>


//#pragma optimize("", off)


namespace mint
{
	namespace TestReflection
	{
		class StructOfArray
		{
			REFLECTION_CLASS(StructOfArray)

		public:
			StructOfArray() { initializeReflection(); }

		public:
			REFLECTION_MEMBER_ARRAY(byte, _arr, 3)
			
		private:
			REFLECTION_BIND_BEGIN
			REFLECTION_BIND(_arr)
			REFLECTION_BIND_END
		};

		bool test()
		{
			const ReflectionData& refl1 = ReflectionTesterOuter::getReflectionData();
			const ReflectionData& refl2 = ReflectionTesterInner::getReflectionData();
			const ReflectionData& refl3 = StructOfArray::getReflectionData();

			StructOfArray struct_of_array;
			struct_of_array._arr[0] = 2;
			struct_of_array._arr[1] = 5;
			struct_of_array._arr[2] = 7;

			ReflectionTesterOuter outer0;
			outer0._id = 0xAABBCCDD;
			outer0._inner._ui = 0xEEEEEEEE;
			outer0._inner._f = 8.25f;
			outer0._inner._str = "CHANGED_VALUE!";
			outer0._uis.push_back(0xFE);
			outer0._uis.push_back(0xDC);
			outer0._uis.push_back(0xBA);
			ReflectionTesterInner inner0;

			Serializer serializer;
			serializer.serialize(outer0, "assets/serialization_test_outer0.bin");
			serializer.serialize(inner0, "assets/serialization_test_inner0.bin");
			serializer.serialize(Float3(1, 2, 3), "assets/serialization_test_float3_0.bin");
			serializer.serialize(struct_of_array, "assets/serialization_test_struct_of_array.bin");

			ReflectionTesterOuter outer1;
			Float3 float3_1 = Float3(9, 9, 9);
			serializer.deserialize("assets/serialization_test_outer0.bin", outer1);
			//serializer.deserialize("assets/serialization_test_outer0.bin", inner0); // This line must fail!!!
			serializer.deserialize("assets/serialization_test_float3_0.bin", float3_1);
			serializer.deserialize("assets/serialization_test_struct_of_array.bin", struct_of_array);
			return true;
		}
	}
}
