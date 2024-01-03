#pragma once


#ifndef _MINT_GAME_COLLISION_COMPONENT_H_
#define _MINT_GAME_COLLISION_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintPhysics/Include/Intersection.h>
#include <MintGame/Include/ObjectComponent.h>


namespace mint
{
	namespace Game
	{
		class Collision2DComponent final : public ObjectComponent
		{
		public:
			Collision2DComponent();
			virtual ~Collision2DComponent();

		public:
			void SetCollisionShape2D(const SharedPtr<Physics::CollisionShape2D>& collisionShape2D);
			const SharedPtr<Physics::CollisionShape2D>& GetCollisionShape2D() const noexcept;

		private:
			SharedPtr<Physics::CollisionShape2D> _collisionShape2D;
		};
	}
}


#endif // !_MINT_GAME_COLLISION_COMPONENT_H_
