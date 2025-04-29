#pragma once


#ifndef _MINT_GAME_COLLISION_COMPONENT_H_
#define _MINT_GAME_COLLISION_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintPhysics/Include/Intersection.h>


namespace mint
{
	namespace Physics
	{
		class CollisionShape2D;
	}

	namespace Game
	{
		struct Collision2DComponent
		{
			SharedPtr<Physics::CollisionShape2D> _collisionShape2D;
		};
	}
}


#endif // !_MINT_GAME_COLLISION_COMPONENT_H_
