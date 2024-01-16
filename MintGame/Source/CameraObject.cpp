﻿#include <MintGame/Include/CameraObject.h>
#include <MintGame/Include/ObjectPool.hpp>

#include <MintPlatform/Include/InputContext.h>


namespace mint
{
	namespace Game
	{
		CameraObject::CameraObject(const ObjectPool* const objectPool)
			: Object(objectPool, ObjectType::CameraObject)
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

		CameraObject::~CameraObject()
		{
			__noop;
		}

		void CameraObject::SetPerspectiveCamera(const float fov, const float nearZ, const float farZ, const float screenRatio)
		{
			_usePerspectiveProjection = true;

			_fov = fov;
			_nearZ = nearZ;
			_farZ = farZ;
			_screenRatio = screenRatio;

			UpdateProjectionMatrix();
		}

		void CameraObject::SetOrthographic2DCamera(const Float2& screenSize)
		{
			_usePerspectiveProjection = false;

			_screenSize = screenSize;

			UpdateProjectionMatrix();
		}

		void CameraObject::UpdateScreenSize(const Float2& screenSize)
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

		void CameraObject::UpdateProjectionMatrix() noexcept
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

		void CameraObject::SteerDefault(const InputContext& inputContext, const bool is3DMode)
		{
			if (is3DMode)
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
			else
			{
				if (inputContext.IsKeyDown(KeyCode::Q) == true)
				{
					Move(CameraObject::MoveDirection::Forward);
				}
				if (inputContext.IsKeyDown(KeyCode::E) == true)
				{
					Move(CameraObject::MoveDirection::Backward);
				}
				if (inputContext.IsKeyDown(KeyCode::W) == true)
				{
					Move(CameraObject::MoveDirection::Upward);
				}
				if (inputContext.IsKeyDown(KeyCode::S) == true)
				{
					Move(CameraObject::MoveDirection::Downward);
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

			if (is3DMode == true && inputContext.IsMousePointerMoved() == true)
			{
				if (inputContext.IsMouseButtonDown(MouseButton::Right) == true)
				{
					RotateByMouseDelta(inputContext.GetMouseDeltaPosition());
				}
			}
		}

		void CameraObject::Move(const MoveDirection moveDirection)
		{
			const float deltaTimeSec = _objectPool->GetDeltaTimeSec();

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
			_pitch += angle;
			_pitch = Math::LimitAngleToPositiveNegativeTwoPiRotation(_pitch);
		}

		void CameraObject::RotateYaw(const float angle)
		{
			_yaw += angle;
			_yaw = Math::LimitAngleToPositiveNegativeTwoPiRotation(_yaw);
		}

		void CameraObject::RotateByMouseDelta(const Float2& mouseDelta)
		{
			static float factor = 0.005f;

			const float handnessSign = GetHandednessSign();
			RotatePitch(mouseDelta._y * factor);
			RotateYaw(mouseDelta._x * factor * handnessSign);
		}

		Float4x4 CameraObject::GetViewMatrix() const noexcept
		{
			const Float4x4& rotationMatrix = GetRotationMatrix();
			return rotationMatrix.Transpose() * Float4x4::TranslationMatrix(-GetObjectTransform()._translation);
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
