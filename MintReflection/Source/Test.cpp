#include <MintReflection/Include/Test.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/String.hpp>

#include <MintReflection/Include/Reflection.h>
#include <MintReflection/Include/Reflection.hpp>

#include <MintMath/Include/Float3.h>


namespace mint
{
	namespace TestReflection
	{
		bool test()
		{
			const ReflectionData& refl1 = ReflectionTesterOuter::getReflectionData();
			const ReflectionData& refl2 = ReflectionTesterInner::getReflectionData();

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

			ReflectionTesterOuter outer1;
			Float3 float3_1 = Float3(9, 9, 9);
			serializer.deserialize("assets/serialization_test_outer0.bin", outer1);
			//serializer.deserialize("assets/serialization_test_outer0.bin", inner0); // This line fails!!!
			serializer.deserialize("assets/serialization_test_float3_0.bin", float3_1);
			return true;
		}
	}
}
