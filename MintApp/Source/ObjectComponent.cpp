#include <MintApp/Include/ObjectComponent.h>

#include <MintPlatform/Include/InputContext.h>
#include <MintApp/Include/Object.h>


namespace mint
{
	std::atomic<uint32> ObjectComponent::_nextRawID = 0;
	ObjectComponent::ObjectComponent(const ObjectComponentType type)
		: _ownerObject{ nullptr }
		, _type{ type }
	{
		_id.Assign(ObjectComponent::_nextRawID);

		++ObjectComponent::_nextRawID;
	}


#pragma region CameraComponent
	CameraComponent::CameraComponent()
		: ObjectComponent(ObjectComponentType::CameraComponent)
		, _usePerspectiveProjection{ true }
		, _isRightHanded{ true }
		, _fov{ Math::ToRadian(60.0f) }
		, _nearZ{ 1.0f }
		, _farZ{ 100.0f }
		, _screenRatio{ 1.0f }
		, _pitch{ 0.0f }
		, _yaw{ 0.0f }
		, _roll{ 0.0f }
		, _moveSpeed{ MoveSpeed::x8_0 }
		, _isBoostMode{ false }
	{
		UpdateProjectionMatrix();
	}

	void CameraComponent::SetPerspectiveCamera(const float fov, const float nearZ, const float farZ, const float screenRatio)
	{
		_usePerspectiveProjection = true;

		_fov = fov;
		_nearZ = nearZ;
		_farZ = farZ;
		_screenRatio = screenRatio;

		UpdateProjectionMatrix();
	}

	void CameraComponent::SetOrthographic2DCamera(const Float2& screenSize)
	{
		_usePerspectiveProjection = false;

		_screenSize = screenSize;

		UpdateProjectionMatrix();
	}

	void CameraComponent::UpdateScreenSize(const Float2& screenSize)
	{
		if (_usePerspectiveProjection)
		{
			_screenRatio = (screenSize._x / screenSize._y);
		}
		else
		{
			_screenSize = screenSize;
		}

		UpdateProjectionMatrix();
	}

	void CameraComponent::UpdateProjectionMatrix() noexcept
	{
		if (_usePerspectiveProjection)
		{
			_projectionMatrix = Float4x4::ProjectionMatrixPerspectiveYUP(_isRightHanded, _fov, _nearZ, _farZ, _screenRatio);
		}
		else
		{
			_projectionMatrix = Float4x4::ProjectionMatrix2DNormal(_screenSize._x, _screenSize._y);
		}
	}

	void CameraComponent::SteerDefault(const float deltaTime, const InputContext& inputContext, const bool is3DMode)
	{
		if (is3DMode)
		{
			if (inputContext.IsKeyDown(KeyCode::Q) == true)
			{
				Move(CameraComponent::MoveDirection::Upward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::E) == true)
			{
				Move(CameraComponent::MoveDirection::Downward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::W) == true)
			{
				Move(CameraComponent::MoveDirection::Forward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::S) == true)
			{
				Move(CameraComponent::MoveDirection::Backward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::A) == true)
			{
				Move(CameraComponent::MoveDirection::Leftward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::D) == true)
			{
				Move(CameraComponent::MoveDirection::Rightward, deltaTime);
			}
		}
		else
		{
			if (inputContext.IsKeyDown(KeyCode::Q) == true)
			{
				Move(CameraComponent::MoveDirection::Forward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::E) == true)
			{
				Move(CameraComponent::MoveDirection::Backward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::W) == true)
			{
				Move(CameraComponent::MoveDirection::Upward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::S) == true)
			{
				Move(CameraComponent::MoveDirection::Downward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::A) == true)
			{
				Move(CameraComponent::MoveDirection::Leftward, deltaTime);
			}
			if (inputContext.IsKeyDown(KeyCode::D) == true)
			{
				Move(CameraComponent::MoveDirection::Rightward, deltaTime);
			}
		}

		if (is3DMode == true && inputContext.IsMousePointerMoved() == true)
		{
			if (inputContext.IsMouseButtonDown(MouseButton::Right) == true)
			{
				RotateByMouseDelta(inputContext.GetMouseDeltaPosition());
			}
		}
	}

	void CameraComponent::Move(const MoveDirection moveDirection, const float deltaTime)
	{
		const float handnessSign = GetHandednessSign();
		const Float3& leftDirection = Float3::Cross(_forwardDirection, Float3::kAxisY) * handnessSign;
		const Float3& upDirection = Float3::Cross(leftDirection, _forwardDirection) * handnessSign;

		const float moveSpeedFloat = GetMoveSpeedAsFloat((_isBoostMode) ? GetFasterMoveSpeed(GetFasterMoveSpeed(_moveSpeed)) : _moveSpeed);
		Transform& transform = _ownerObject->GetObjectTransform();
		switch (moveDirection)
		{
		case CameraComponent::MoveDirection::Forward:
			transform._translation += _forwardDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraComponent::MoveDirection::Backward:
			transform._translation -= _forwardDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraComponent::MoveDirection::Leftward:
			transform._translation += leftDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraComponent::MoveDirection::Rightward:
			transform._translation -= leftDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraComponent::MoveDirection::Upward:
			transform._translation += upDirection * moveSpeedFloat * deltaTime;
			break;
		case CameraComponent::MoveDirection::Downward:
			transform._translation -= upDirection * moveSpeedFloat * deltaTime;
			break;
		default:
			break;
		}
	}

	void CameraComponent::IncreaseMoveSpeed() noexcept
	{
		_moveSpeed = GetFasterMoveSpeed(_moveSpeed);
	}

	void CameraComponent::DecreaseMoveSpeed() noexcept
	{
		_moveSpeed = GetSlowerMoveSpeed(_moveSpeed);
	}

	void CameraComponent::SetBoostMode(const bool isBoostMode) noexcept
	{
		_isBoostMode = isBoostMode;
	}

	void CameraComponent::RotatePitch(const float angle)
	{
		_pitch += angle;
		_pitch = Math::LimitAngleToPositiveNegativeTwoPiRotation(_pitch);
	}

	void CameraComponent::RotateYaw(const float angle)
	{
		_yaw += angle;
		_yaw = Math::LimitAngleToPositiveNegativeTwoPiRotation(_yaw);
	}

	void CameraComponent::RotateByMouseDelta(const Float2& mouseDelta)
	{
		static float factor = 0.005f;

		const float handnessSign = GetHandednessSign();
		RotatePitch(mouseDelta._y * factor);
		RotateYaw(mouseDelta._x * factor * handnessSign);
	}

	Float4x4 CameraComponent::GetViewMatrix() const noexcept
	{
		const Float4x4& rotationMatrix = GetRotationMatrix();
		return rotationMatrix.Transpose() * Float4x4::TranslationMatrix(-_ownerObject->GetObjectTransform()._translation);
	}

	const Float4x4& CameraComponent::GetProjectionMatrix() const noexcept
	{
		return _projectionMatrix;
	}

	Float4x4 CameraComponent::GetRotationMatrix() const noexcept
	{
		const float handnessSign = GetHandednessSign();
		const Float3 kBaseForward = Float3::kAxisZ * handnessSign;
		const Float3& forwardDirectionXz = Float4x4::RotationMatrixY(_yaw) * kBaseForward;
		const Float3& leftDirection = Float3::CrossAndNormalize(forwardDirectionXz, Float3::kAxisY) * handnessSign;
		_forwardDirection = Float4x4::RotationMatrixAxisAngle(leftDirection * -handnessSign, _pitch) * forwardDirectionXz;
		const Float3& upDirection = Float3::CrossAndNormalize(leftDirection, _forwardDirection) * handnessSign;
		return Float4x4::RotationMatrixFromAxes(-leftDirection, upDirection, _forwardDirection * handnessSign);
	}

	float CameraComponent::GetHandednessSign() const noexcept
	{
		return (_isRightHanded ? -1.0f : +1.0f);
	}
#pragma endregion
}
