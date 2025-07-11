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
				Physics2D::CircleCollisionShape a = Physics2D::CircleCollisionShape(Float2(70, 64), 2);
				Physics2D::CircleCollisionShape b = Physics2D::CircleCollisionShape(Float2(64, 64), 16);
				MINT_ASSURE(Physics2D::Intersect2D_GJK(a, b) == true);
			}

			{
				Physics2D::PointCollisionShape a = Physics2D::PointCollisionShape(Float2(80, 64));
				Physics2D::CircleCollisionShape b = Physics2D::CircleCollisionShape(Float2(64, 64), 16);
				MINT_ASSURE(Physics2D::Intersect2D_GJK(a, b) == true);
			}

			{
				Physics2D::BoxCollisionShape a = Physics2D::BoxCollisionShape(Float2(32, 16), Transform2D(0.0f, Float2(112, 76)));
				Physics2D::CircleCollisionShape b = Physics2D::CircleCollisionShape(Float2(64, 64), 16);
				MINT_ASSURE(Physics2D::Intersect2D_GJK(a, b) == true);
			}

			{
				Physics2D::BoxCollisionShape a = Physics2D::BoxCollisionShape(Float2(16, 16), Transform2D::GetIdentity());
				Physics2D::CircleCollisionShape b = Physics2D::CircleCollisionShape(Float2(0, 0), 1);
				MINT_ASSURE(Physics2D::Intersect2D_GJK(a, b) == true);
			}

			return true;
		}
	}
}
