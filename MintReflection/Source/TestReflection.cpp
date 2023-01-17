﻿#include <MintReflection/Include/TestReflection.h>

#include <MintReflection/Include/AllHeaders.h>
#include <MintReflection/Include/AllHpps.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/String.hpp>
#include <MintMath/Include/Float3.h>


//#pragma optimize("", off)


namespace mint
{
	namespace TestReflection
	{
		class StructOfArrays
		{
			REFLECTION_CLASS(StructOfArrays)

		public:
			StructOfArrays() { initializeReflection(); }

		public:
			REFLECTION_MEMBER_ARRAY(byte, _arr, 3)
			REFLECTION_MEMBER_ARRAY(StringA, _strs, 2)
			
		private:
			REFLECTION_BIND_BEGIN
			REFLECTION_BIND(_arr)
			REFLECTION_BIND(_strs)
			REFLECTION_BIND_END
		};

		bool test()
		{
			const ReflectionData& refl1 = ReflectionTesterOuter::getReflectionData();
			const ReflectionData& refl2 = ReflectionTesterInner::getReflectionData();
			const ReflectionData& refl3 = StructOfArrays::getReflectionData();

			ReflectionTesterOuter outer0;
			outer0._id = 0xAABBCCDD;
			outer0._inner._ui = 0xEEEEEEEE;
			outer0._inner._f = 8.25f;
			outer0._inner._str = "CHANGED_VALUE!";
			outer0._uis.push_back(0xFE);
			outer0._uis.push_back(0xDC);
			outer0._uis.push_back(0xBA);
			ReflectionTesterInner inner0;
			Float3 float3_0 = Float3(1, 2, 3);
			StructOfArrays struct_of_arrays0;
			struct_of_arrays0._arr[0] = 2;
			struct_of_arrays0._arr[1] = 5;
			struct_of_arrays0._arr[2] = 7;
			struct_of_arrays0._strs[0] = "ABCD";
			struct_of_arrays0._strs[1] = "WXYZ";

			BinarySerializer serializer;
			serializer.serialize(outer0, "assets/serialization_test_outer0.bin");
			serializer.serialize(inner0, "assets/serialization_test_inner0.bin");
			serializer.serialize(float3_0, "assets/serialization_test_float3_0.bin");
			serializer.serialize(struct_of_arrays0, "assets/serialization_test_struct_of_arrays0.bin");

			ReflectionTesterOuter outer1;
			Float3 float3_1 = Float3(9, 9, 9);
			StructOfArrays struct_of_arrays1;
			serializer.deserialize("assets/serialization_test_outer0.bin", outer1);
			//serializer.deserialize("assets/serialization_test_outer0.bin", inner0); // This line must fail!!!
			serializer.deserialize("assets/serialization_test_float3_0.bin", float3_1);
			serializer.deserialize("assets/serialization_test_struct_of_arrays0.bin", struct_of_arrays1);

			//MINT_ASSURE(outer0 == outer1);
			MINT_ASSURE(float3_0 == float3_1);
			//MINT_ASSURE(struct_of_arrays0 == struct_of_arrays1);

			JSONSerializer jsonSerializer;
			jsonSerializer.serialize(outer0, "assets/serialization_test_outer0.json");
			jsonSerializer.serialize(inner0, "assets/serialization_test_inner0.json");
			//jsonSerializer.serialize(float3_0, "assets/serialization_test_float3_0.json"); // This line must fail!!!
			jsonSerializer.serialize(struct_of_arrays0, "assets/serialization_test_struct_of_arrays0.json");
			return true;
		}
	}
}
