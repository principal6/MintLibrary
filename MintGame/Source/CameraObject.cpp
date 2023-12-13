#include <MintGame/Include/CameraObject.h>
#include <MintGame/Include/ObjectPool.hpp>

#include <MintPlatform/Include/InputContext.h>


namespace mint
{
	namespace Game
	{
		CameraObject::CameraObject(const ObjectPool* const objectPool)
			: Object(objectPool, ObjectType::CameraObject)
			, _isRightHanded{ true }
			, _fov{ Math::ToRadian(60.0f) }
			, _nearZ{ 0.1f }
			, _farZ{ 1000.0f }
			, _screenRatio{ 1.0f }
			, _pitch{ 0.0f }
			, _yaw{ 0.0f }
			, _roll{ 0.0f }
			, _moveSpeed{ MoveSpeed::x8_0 }
			, _isBoostMode{ false }
			, _rotationFactor{ 0.005f }
		{
			UpdatePerspectiveMatrix();
		}

		CameraObject::~CameraObject()
		{
			__noop;
		}

		void CameraObject::SetPerspectiveFov(const float fov)
		{
			_fov = fov;
			UpdatePerspectiveMatrix();
		}

		void CameraObject::SetPerspectiveZRange(const float nearZ, const float farZ)
		{
			_nearZ = nearZ;
			_farZ = farZ;
			UpdatePerspectiveMatrix();
		}

		void CameraObject::SetPerspectiveScreenRatio(const float screenRatio)
		{
			_screenRatio = screenRatio;
			UpdatePerspectiveMatrix();
		}

		void CameraObject::UpdatePerspectiveMatrix() noexcept
		{
			_projectionMatrix = Float4x4::ProjectionMatrixPerspectiveYUP(_isRightHanded, _fov, _nearZ, _farZ, _screenRatio);
		}

		void CameraObject::Steer(const InputContext& inputContext, const bool isMoveLocked)
		{
			if (isMoveLocked == false)
			{
				if (inputContext.IsKeyDown(KeyCode::Q) == true)
				{
					Move(CameraObject::MoveDirection::Upward);
				}

				if (inputContext.IsKeyDown(KeyCode::E) == true)
				{
					Move(CameraObject::MoveDirection::Downward);
				}

				if (inputContext.IsKeyDown(KeyCode::W) == true)
				{
					Move(CameraObject::MoveDirection::Forward);
				}

				if (inputContext.IsKeyDown(KeyCode::S) == true)
				{
					Move(CameraObject::MoveDirection::Backward);
				}

				if (inputContext.IsKeyDown(KeyCode::A) == true)
				{
					Move(CameraObject::MoveDirection::Leftward);
				}

				if (inputContext.IsKeyDown(KeyCode::D) == true)
				{
					Move(CameraObject::MoveDirection::Rightward);
				}
			}

			if (inputContext.IsMousePointerMoved() == true)
			{
				if (inputContext.IsMouseButtonDown(MouseButton::Right) == true)
				{
					RotateByMouseDelta(inputContext.GetMouseDeltaPosition());
				}
			}
		}

		void CameraObject::Move(const MoveDirection moveDirection)
		{
			const float deltaTimeSec = GetDeltaTimeSec();

			const float handnessSign = GetHandednessSign();
			const Float3& leftDirection = Float3::Cross(_forwardDirection, Float3::kAxisY) * handnessSign;
			const Float3& upDirection = Float3::Cross(leftDirection, _forwardDirection) * handnessSign;

			const float moveSpeedFloat = GetMoveSpeedAsFloat((_isBoostMode) ? GetFasterMoveSpeed(GetFasterMoveSpeed(_moveSpeed)) : _moveSpeed);
			Transform& transform = GetObjectTransform();
			switch (moveDirection)
			{
			case CameraObject::MoveDirection::Forward:
				transform._translation += _forwardDirection * moveSpeedFloat * deltaTimeSec;
				break;
			case CameraObject::MoveDirection::Backward:
				transform._translation -= _forwardDirection * moveSpeedFloat * deltaTimeSec;
				break;
			case CameraObject::MoveDirection::Leftward:
				transform._translation += leftDirection * moveSpeedFloat * deltaTimeSec;
				break;
			case CameraObject::MoveDirection::Rightward:
				transform._translation -= leftDirection * moveSpeedFloat * deltaTimeSec;
				break;
			case CameraObject::MoveDirection::Upward:
				transform._translation += upDirection * moveSpeedFloat * deltaTimeSec;
				break;
			case CameraObject::MoveDirection::Downward:
				transform._translation -= upDirection * moveSpeedFloat * deltaTimeSec;
				break;
			default:
				break;
			}
		}

		void CameraObject::IncreaseMoveSpeed() noexcept
		{
			_moveSpeed = GetFasterMoveSpeed(_moveSpeed);
		}

		void CameraObject::DecreaseMoveSpeed() noexcept
		{
			_moveSpeed = GetSlowerMoveSpeed(_moveSpeed);
		}

		void CameraObject::SetBoostMode(const bool isBoostMode) noexcept
		{
			_isBoostMode = isBoostMode;
		}

		void CameraObject::RotatePitch(const float angle)
		{
			_pitch += angle * _rotationFactor;
			_pitch = Math::LimitAngleToPositiveNegativeTwoPiRotation(_pitch);
		}

		void CameraObject::RotateYaw(const float angle)
		{
			_yaw += angle * _rotationFactor;
			_yaw = Math::LimitAngleToPositiveNegativeTwoPiRotation(_yaw);
		}

		void CameraObject::RotateByMouseDelta(const Float2& mouseDelta)
		{
			const float handnessSign = GetHandednessSign();
			RotatePitch(mouseDelta._y);
			RotateYaw(mouseDelta._x * handnessSign);
		}

		Float4x4 CameraObject::GetViewMatrix() const noexcept
		{
			const Float4x4& RotationMatrix = GetRotationMatrix();
			return RotationMatrix.Transpose() * Float4x4::TranslationMatrix(-GetObjectTransform()._translation);
		}

		const Float4x4& CameraObject::GetProjectionMatrix() const noexcept
		{
			return _projectionMatrix;
		}

		Float4x4 CameraObject::GetRotationMatrix() const noexcept
		{
			const float handnessSign = GetHandednessSign();
			const Float3 kBaseForward = Float3::kAxisZ * handnessSign;
			const Float3& forwardDirectionXz = Float4x4::RotationMatrixY(_yaw) * kBaseForward;
			const Float3& leftDirection = Float3::CrossAndNormalize(forwardDirectionXz, Float3::kAxisY) * handnessSign;
			_forwardDirection = Float4x4::RotationMatrixAxisAngle(leftDirection * -handnessSign, _pitch) * forwardDirectionXz;
			const Float3& upDirection = Float3::CrossAndNormalize(leftDirection, _forwardDirection) * handnessSign;
			return Float4x4::RotationMatrixFromAxes(-leftDirection, upDirection, _forwardDirection * handnessSign);
		}

		float CameraObject::GetHandednessSign() const noexcept
		{
			return (_isRightHanded ? -1.0f : +1.0f);
		}
	}
}
