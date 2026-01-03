#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_H_
#define _MINT_APP_SCENE_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintMath/Include/Transform.h>
#include <MintECS/Include/Entity.h>
#include <MintECS/Include/EntityRegistry.h>


namespace mint
{
	class InputContext;
}

namespace mint
{
	class SceneObject : public ECS::EntityBase<uint64, 32> {};

	template<typename ComponentType>
	using SceneObjectComponentPool = ECS::EntityComponentPool<SceneObject, ComponentType>;

	struct TransformComponent
	{
		Transform _transform;
	};

	enum class CameraMoveDirection : uint8
	{
		Forward,
		Backward,
		Leftward,
		Rightward,
		Upward,
		Downward
	};
	enum class CameraMoveSpeed : uint8
	{
		x0_125,
		x0_25,
		x0_5,
		x1_0,
		x2_0,
		x4_0,
		x8_0,
		x16_0,
		COUNT
	};
	struct CameraComponent
	{
		Float4x4 _projectionMatrix;
		mutable Float3 _forwardDirection;
		bool _usePerspectiveProjection = true;
		bool _isRightHanded = true;
		float _fov = Math::ToRadian(60.0f);
		union
		{
			struct
			{
				float _nearZ;
				float _farZ;
			};
			Float2 _screenSize{ 1.0f, 100.0f };
		};
		float _screenRatio = 1.0f;
		float _pitch = 0.0f;
		float _yaw = 0.0f;
		float _roll = 0.0f;
		CameraMoveSpeed _moveSpeed = CameraMoveSpeed::x8_0;
		bool _isBoostMode = false;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_H_
