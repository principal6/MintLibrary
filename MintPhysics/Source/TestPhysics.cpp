#include <MintPhysics/Include/AllHeaders.h>
#include <MintPhysics/Include/AllHpps.h>


namespace mint
{
	namespace TestPhysics
	{
		bool TestIntersections()
		{
			{
				Physics::CircleShape2D a = Physics::CircleShape2D(Float2(70, 64), 2);
				Physics::CircleShape2D b = Physics::CircleShape2D(Float2(64, 64), 16);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}
			
			{
				Physics::PointShape2D a = Physics::PointShape2D(Float2(80, 64));
				Physics::CircleShape2D b = Physics::CircleShape2D(Float2(64, 64), 16);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}

			return true;
		}
	}
}
