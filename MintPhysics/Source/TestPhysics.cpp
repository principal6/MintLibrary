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
				Physics::CircleCollisionShape2D a = Physics::CircleCollisionShape2D(Float2(70, 64), 2);
				Physics::CircleCollisionShape2D b = Physics::CircleCollisionShape2D(Float2(64, 64), 16);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}
			
			{
				Physics::PointCollisionShape2D a = Physics::PointCollisionShape2D(Float2(80, 64));
				Physics::CircleCollisionShape2D b = Physics::CircleCollisionShape2D(Float2(64, 64), 16);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}
			
			{
				Physics::BoxCollisionShape2D a = Physics::BoxCollisionShape2D(Float2(112, 76), Float2(32, 16), 0.0f);
				Physics::CircleCollisionShape2D b = Physics::CircleCollisionShape2D(Float2(64, 64), 16);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}
			
			{
				Physics::BoxCollisionShape2D a = Physics::BoxCollisionShape2D(Float2(0, 0), Float2(16, 16), 0.0f);
				Physics::CircleCollisionShape2D b = Physics::CircleCollisionShape2D(Float2(0, 0), 1);
				MINT_ASSURE(Physics::Intersect2D_GJK(a, b) == true);
			}

			return true;
		}
	}
}
