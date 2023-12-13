#pragma once


#ifndef _MINT_GAME_TRANSFORM_COMPONENT_H_
#define _MINT_GAME_TRANSFORM_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Transform.h>

#include <MintGame/Include/ObjectComponent.h>


namespace mint
{
	namespace Game
	{
		class TransformComponent : public ObjectComponent
		{
		public:
			TransformComponent() : TransformComponent(ObjectComponentType::TransformComponent) { __noop; }
			TransformComponent(const ObjectComponentType derivedType) : ObjectComponent(derivedType) { __noop; }
			virtual ~TransformComponent() { __noop; }

		public:
			Transform _transform;
		};

		class Transform2DComponent : public ObjectComponent
		{
		public:
			Transform2DComponent() : Transform2DComponent(ObjectComponentType::Transform2DComponent) { __noop; }
			Transform2DComponent(const ObjectComponentType derivedType) : ObjectComponent(derivedType), _rotation{ 0.0f } { __noop; }
			virtual ~Transform2DComponent() { __noop; }

		public:
			Float2 _position;
			float _rotation;
		};
	}
}


#endif // !_MINT_GAME_TRANSFORM_COMPONENT_H_
