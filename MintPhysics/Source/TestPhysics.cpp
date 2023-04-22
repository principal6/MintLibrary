#include <MintPhysics/Include/TestPhysics.h>
#include <MintPhysics/Include/AllHeaders.h>
#include <MintPhysics/Include/AllHpps.h>


namespace mint
{
	namespace TestPhysics
	{
		bool Test()
		{
			MINT_ASSURE(TestIntersections());
			return true;
		}

		bool TestIntersections()
		{
			{
				Physics::GJKCircleShape2D a = Physics::GJKCircleShape2D(Float2(70, 64), 2);
				Physics::GJKCircleShape2D b = Physics::GJKCircleShape2D(Float2(64, 64), 16);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}
			
			{
				Physics::GJKPointShape2D a = Physics::GJKPointShape2D(Float2(80, 64));
				Physics::GJKCircleShape2D b = Physics::GJKCircleShape2D(Float2(64, 64), 16);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}

			return true;
		}
	}
}
