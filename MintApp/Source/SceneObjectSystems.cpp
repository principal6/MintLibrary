#include <MintApp/Include/SceneObjectSystems.h>
#include <MintApp/Include/SceneObject.h>
#include <MintApp/Include/SceneObjectRegistry.hpp>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/ContiguousHashMap.hpp>
#include <MintPlatform/Include/InputContext.h>


namespace mint
{
	void SceneObjectComponentHelpers::MakePerspectiveCamera(float fov, float nearZ, float farZ, float screenRatio, CameraComponent& cameraComponent)
	{
		cameraComponent._usePerspectiveProjection = true;

		cameraComponent._fov = fov;
		cameraComponent._nearZ = nearZ;
		cameraComponent._farZ = farZ;
		cameraComponent._screenRatio = screenRatio;

		SceneObjectCameraSystem::UpdateProjectionMatrix(cameraComponent);
	}

	void SceneObjectComponentHelpers::MakeOrthographic2DCamera(const Float2& screenSize, CameraComponent& cameraComponent)
	{
		cameraComponent._usePerspectiveProjection = false;

		cameraComponent._screenSize = screenSize;

		SceneObjectCameraSystem::UpdateProjectionMatrix(cameraComponent);
	}

	void SceneObjectComponentHelpers::RotateCameraPitch(float deltaPitch, CameraComponent& cameraComponent)
	{
		cameraComponent._pitch += deltaPitch;
		cameraComponent._pitch = Math::LimitAngleToPositiveNegativeTwoPiRotation(cameraComponent._pitch);
	}

	void SceneObjectComponentHelpers::RotateCameraYaw(float deltaYaw, CameraComponent& cameraComponent)
	{
		cameraComponent._yaw += deltaYaw;
		cameraComponent._yaw = Math::LimitAngleToPositiveNegativeTwoPiRotation(cameraComponent._yaw);
	}


	void SceneObjectCameraSystem::UpdateProjectionMatrix(CameraComponent& cameraComponent)
	{
		if (cameraComponent._usePerspectiveProjection)
		{
			cameraComponent._projectionMatrix = Float4x4::ProjectionMatrixPerspectiveYUP(cameraComponent._isRightHanded, cameraComponent._fov,
				cameraComponent._nearZ, cameraComponent._farZ, cameraComponent._screenRatio);
		}
		else
		{
			cameraComponent._projectionMatrix = Float4x4::ProjectionMatrix2DNormal(cameraComponent._screenSize._x, cameraComponent._screenSize._y);
		}
	}

	void SceneObjectCameraSystem::UpdateScreenSize(CameraComponent& cameraComponent, const Float2& screenSize)
	{
		if (cameraComponent._usePerspectiveProjection)
		{
			cameraComponent._screenRatio = (screenSize._x / screenSize._y);
		}
		else
		{
			cameraComponent._screenSize = screenSize;
		}

		SceneObjectCameraSystem::UpdateProjectionMatrix(cameraComponent);
	}

	void SceneObjectCameraSystem::UpdateSystem(const Vector<SceneObject>& sceneObjects)
	{
		SceneObjectComponentPool<CameraComponent>& cameraComponentPool = _sceneObjectRegistry.GetComponentPool<CameraComponent>();
		for (const SceneObject& sceneObject : sceneObjects)
		{
			CameraComponent* const cameraComponent = cameraComponentPool.GetComponent(sceneObject);
		}
	}

	void SceneObjectCameraSystem::UpdateScreenSize(const Float2& screenSize)
	{
		SceneObjectComponentPool<CameraComponent>& cameraComponentPool = _sceneObjectRegistry.GetComponentPool<CameraComponent>();
		ContiguousHashMap<SceneObject, CameraComponent>& cameraComponentMap = cameraComponentPool.GetComponentMap();
		for (CameraComponent& cameraComponent : cameraComponentMap)
		{
			UpdateScreenSize(cameraComponent, screenSize);
		}
	}

	Float4x4 SceneObjectCameraSystem::MakeViewMatrix(const SceneObject& cameraObject) const
	{
		SceneObjectComponentPool<CameraComponent>& cameraComponentPool = _sceneObjectRegistry.GetComponentPool<CameraComponent>();
		CameraComponent* const cameraComponent = cameraComponentPool.GetComponent(cameraObject);
		const float handnessSign = GetHandednessSign(*cameraComponent);
		const Float3 kBaseForward = Float3::kAxisZ * handnessSign;
		const Float3& forwardDirectionXz = Float4x4::RotationMatrixY(cameraComponent->_yaw) * kBaseForward;
		const Float3& leftDirection = Float3::CrossAndNormalize(forwardDirectionXz, Float3::kAxisY) * handnessSign;
		cameraComponent->_forwardDirection = Float4x4::RotationMatrixAxisAngle(leftDirection * -handnessSign, cameraComponent->_pitch) * forwardDirectionXz;
		const Float3& upDirection = Float3::CrossAndNormalize(leftDirection, cameraComponent->_forwardDirection) * handnessSign;
		const Float4x4& rotationMatrix = Float4x4::RotationMatrixFromAxes(-leftDirection, upDirection, cameraComponent->_forwardDirection * handnessSign);
		const TransformComponent& transformComponent = _sceneObjectRegistry.GetComponentMust<TransformComponent>(cameraObject);
		return rotationMatrix.Transpose() * Float4x4::TranslationMatrix(-transformComponent._transform._translation);
	}

	void SceneObjectCameraSystem::IncreaseCurrentCameraMoveSpeed()
	{
		SceneObjectComponentPool<CameraComponent>& cameraComponentPool = _sceneObjectRegistry.GetComponentPool<CameraComponent>();
		CameraComponent* const cameraComponent = cameraComponentPool.GetComponent(_currentCameraObject);
		cameraComponent->_moveSpeed = GetFasterMoveSpeed(cameraComponent->_moveSpeed);
	}

	void SceneObjectCameraSystem::DecreaseCurrentCameraMoveSpeed()
	{
		SceneObjectComponentPool<CameraComponent>& cameraComponentPool = _sceneObjectRegistry.GetComponentPool<CameraComponent>();
		CameraComponent* const cameraComponent = cameraComponentPool.GetComponent(_currentCameraObject);
		cameraComponent->_moveSpeed = GetSlowerMoveSpeed(cameraComponent->_moveSpeed);
	}

	MINT_INLINE constexpr float GetMoveSpeedAsFloat(const CameraMoveSpeed moveSpeed)
	{
		static_assert(static_cast<uint8>(CameraMoveSpeed::x0_125) == 0, "Base 가 달라졌습니다!");
		const uint8 moveSpeedUint8 = static_cast<uint8>(moveSpeed);
		return 0.125f * static_cast<float>(Math::Pow2_Uint32(moveSpeedUint8));
	}

	CameraMoveSpeed SceneObjectCameraSystem::GetFasterMoveSpeed(const CameraMoveSpeed moveSpeed) const
	{
		const uint8 moveSpeedUint8 = static_cast<uint8>(moveSpeed);
		const uint8 moveSpeedMax = static_cast<uint8>(CameraMoveSpeed::COUNT) - 1;
		if (moveSpeedUint8 < moveSpeedMax)
		{
			return static_cast<CameraMoveSpeed>(moveSpeedUint8 + 1);
		}
		return moveSpeed;
	}

	CameraMoveSpeed SceneObjectCameraSystem::GetSlowerMoveSpeed(const CameraMoveSpeed moveSpeed) const
	{
		const uint8 moveSpeedUint8 = static_cast<uint8>(moveSpeed);
		if (moveSpeedUint8 > 0)
		{
			return static_cast<CameraMoveSpeed>(moveSpeedUint8 - 1);
		}
		return moveSpeed;
	}

	void SceneObjectCameraSystem::DefaultSteerCurrentCamera(const float deltaTime, const InputContext& inputContext, const bool is3DMode)
	{
		if (is3DMode)
		{
			if (inputContext.IsKeyDown(KeyCode::Q) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Upward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::E) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Downward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::W) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Forward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::S) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Backward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::A) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Leftward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::D) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Rightward, deltaTime);
			}
		}
		else
		{
			if (inputContext.IsKeyDown(KeyCode::Q) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Forward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::E) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Backward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::W) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Upward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::S) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Downward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::A) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Leftward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::D) == true)
			{
				MoveCurrentCamera(CameraMoveDirection::Rightward, deltaTime);
			}
		}

		if (is3DMode == true && inputContext.IsMousePointerMoved() == true)
		{
			if (inputContext.IsMouseButtonDown(MouseButton::Right) == true)
			{
				RotateCurrentCameraByMouseDelta(inputContext.GetMouseDeltaPosition());
			}
		}
	}

	void SceneObjectCameraSystem::RotateCurrentCameraByMouseDelta(const Float2& mouseDelta)
	{
		static float factor = 0.005f;

		SceneObjectComponentPool<CameraComponent>& cameraComponentPool = _sceneObjectRegistry.GetComponentPool<CameraComponent>();
		CameraComponent* const cameraComponent = cameraComponentPool.GetComponent(_currentCameraObject);
		const float handnessSign = GetHandednessSign(*cameraComponent);
		SceneObjectComponentHelpers::RotateCameraPitch(mouseDelta._y * factor, *cameraComponent);
		SceneObjectComponentHelpers::RotateCameraYaw(mouseDelta._x * factor * handnessSign, *cameraComponent);
	}

	void SceneObjectCameraSystem::MoveCurrentCamera(const CameraMoveDirection& cameraMoveDirection, float deltaTime)
	{
		SceneObjectComponentPool<CameraComponent>& cameraComponentPool = _sceneObjectRegistry.GetComponentPool<CameraComponent>();
		CameraComponent* const cameraComponent = cameraComponentPool.GetComponent(_currentCameraObject);
		const float handnessSign = GetHandednessSign(*cameraComponent);
		const Float3& leftDirection = Float3::Cross(cameraComponent->_forwardDirection, Float3::kAxisY) * handnessSign;
		const Float3& upDirection = Float3::Cross(leftDirection, cameraComponent->_forwardDirection) * handnessSign;

		const float moveSpeedFloat = GetMoveSpeedAsFloat((cameraComponent->_isBoostMode) ? GetFasterMoveSpeed(GetFasterMoveSpeed(cameraComponent->_moveSpeed)) : cameraComponent->_moveSpeed);
		TransformComponent& transformComponent = _sceneObjectRegistry.GetComponentMust<TransformComponent>(_currentCameraObject);
		switch (cameraMoveDirection)
		{
		case CameraMoveDirection::Forward:
			transformComponent._transform._translation += cameraComponent->_forwardDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraMoveDirection::Backward:
			transformComponent._transform._translation -= cameraComponent->_forwardDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraMoveDirection::Leftward:
			transformComponent._transform._translation += leftDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraMoveDirection::Rightward:
			transformComponent._transform._translation -= leftDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraMoveDirection::Upward:
			transformComponent._transform._translation += upDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraMoveDirection::Downward:
			transformComponent._transform._translation -= upDirection * moveSpeedFloat * deltaTime;
			break;
		default:
			break;
		}
	}

	float SceneObjectCameraSystem::GetHandednessSign(const CameraComponent& cameraComponent) const noexcept
	{
		return (cameraComponent._isRightHanded ? -1.0f : +1.0f);
	}
}
