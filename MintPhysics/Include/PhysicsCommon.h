#pragma once


#ifndef _MINT_PHYSICS_PHYSICS_COMMON_H_
#define _MINT_PHYSICS_PHYSICS_COMMON_H_

#include <MintContainer/Include/ID.h>

namespace mint
{
	namespace Physics2D
	{
		class World;
	}
}

namespace mint
{
	namespace PhysicsCommon
	{
		enum class BodyMotionType
		{
			Static,
			Dynamic,
			KeyFramed,
		};
	}
}

namespace mint
{
	namespace Physics2D
	{
		using namespace PhysicsCommon;

		class BodyID : public ID32
		{
			friend World;
			BodyID(uint32 value) : ID32() { Assign(value); }

		public:
			BodyID() = default;
		};

		struct RigidBodyComponent
		{
			BodyID _bodyID;
		};

	}
}

#endif // !_MINT_PHYSICS_PHYSICS_COMMON_H_
