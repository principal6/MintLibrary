#include <MintGame/Include/CollisionComponent.h>
#include <MintContainer/Include/SharedPtr.hpp>


namespace mint
{
	namespace Game
	{
		Collision2DComponent::Collision2DComponent()
			: SceneObjectComponent(SceneObjectComponentType::Collision2DComponent)
		{
			__noop;
		}

		Collision2DComponent::~Collision2DComponent()
		{
			__noop;
		}

		void Collision2DComponent::SetCollisionShape2D(const SharedPtr<Physics::CollisionShape2D>& collisionShape2D)
		{
			_collisionShape2D = collisionShape2D;
		}

		const SharedPtr<Physics::CollisionShape2D>& Collision2DComponent::GetCollisionShape2D() const noexcept
		{
			return _collisionShape2D;
		}
	}
}
