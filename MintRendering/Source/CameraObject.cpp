#include <MintRendering/Include/CameraObject.h>
#include <MintRendering/Include/ObjectPool.hpp>

#include <MintPlatform/Include/InputContext.h>


namespace mint
{
	namespace Rendering
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
			updatePerspectiveMatrix();
		}

		CameraObject::~CameraObject()
		{
			__noop;
		}

		void CameraObject::setPerspectiveFov(const float fov)
		{
			_fov = fov;
			updatePerspectiveMatrix();
		}

		void CameraObject::setPerspectiveZRange(const float nearZ, const float farZ)
		{
			_nearZ = nearZ;
			_farZ = farZ;
			updatePerspectiveMatrix();
		}

		void CameraObject::setPerspectiveScreenRatio(const float screenRatio)
		{
			_screenRatio = screenRatio;
			updatePerspectiveMatrix();
		}

		void CameraObject::updatePerspectiveMatrix() noexcept
		{
			_projectionMatrix = Float4x4::ProjectionMatrixPerspectiveYUP(_isRightHanded, _fov, _nearZ, _farZ, _screenRatio);
		}

		void CameraObject::steer(const Platform::InputContext& inputContext, const bool isMoveLocked)
		{
			if (isMoveLocked == false)
			{
				if (inputContext.isKeyDown(Platform::KeyCode::Q) == true)
				{
					move(Rendering::CameraObject::MoveDirection::Upward);
				}

				if (inputContext.isKeyDown(Platform::KeyCode::E) == true)
				{
					move(Rendering::CameraObject::MoveDirection::Downward);
				}

				if (inputContext.isKeyDown(Platform::KeyCode::W) == true)
				{
					move(Rendering::CameraObject::MoveDirection::Forward);
				}

				if (inputContext.isKeyDown(Platform::KeyCode::S) == true)
				{
					move(Rendering::CameraObject::MoveDirection::Backward);
				}

				if (inputContext.isKeyDown(Platform::KeyCode::A) == true)
				{
					move(Rendering::CameraObject::MoveDirection::Leftward);
				}

				if (inputContext.isKeyDown(Platform::KeyCode::D) == true)
				{
					move(Rendering::CameraObject::MoveDirection::Rightward);
				}
			}

			if (inputContext.isMousePointerMoved() == true)
			{
				if (inputContext.isMouseButtonDown(Platform::MouseButton::Right) == true)
				{
					rotateByMouseDelta(inputContext.getMouseDeltaPosition());
				}
			}
		}

		void CameraObject::move(const MoveDirection moveDirection)
		{
			const float deltaTimeSec = getDeltaTimeSec();

			const float handnessSign = getHandednessSign();
			const Float3& leftDirection = Float3::Cross(_forwardDirection, Float3::kAxisY) * handnessSign;
			const Float3& upDirection = Float3::Cross(leftDirection, _forwardDirection) * handnessSign;

			const float moveSpeedFloat = getMoveSpeedAsFloat((_isBoostMode) ? getFasterMoveSpeed(getFasterMoveSpeed(_moveSpeed)) : _moveSpeed);
			Transform& transform = getObjectTransform();
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

		void CameraObject::increaseMoveSpeed() noexcept
		{
			_moveSpeed = getFasterMoveSpeed(_moveSpeed);
		}

		void CameraObject::decreaseMoveSpeed() noexcept
		{
			_moveSpeed = getSlowerMoveSpeed(_moveSpeed);
		}

		void CameraObject::setBoostMode(const bool isBoostMode) noexcept
		{
			_isBoostMode = isBoostMode;
		}

		void CameraObject::rotatePitch(const float angle)
		{
			_pitch += angle * _rotationFactor;
			_pitch = Math::LimitAngleToPositiveNegativeTwoPiRotation(_pitch);
		}

		void CameraObject::rotateYaw(const float angle)
		{
			_yaw += angle * _rotationFactor;
			_yaw = Math::LimitAngleToPositiveNegativeTwoPiRotation(_yaw);
		}

		void CameraObject::rotateByMouseDelta(const Float2& mouseDelta)
		{
			const float handnessSign = getHandednessSign();
			rotatePitch(mouseDelta._y);
			rotateYaw(mouseDelta._x * handnessSign);
		}

		Float4x4 CameraObject::getViewMatrix() const noexcept
		{
			const Float4x4& RotationMatrix = getRotationMatrix();
			return RotationMatrix.Transpose() * Float4x4::TranslationMatrix(-getObjectTransform()._translation);
		}

		const Float4x4& CameraObject::getProjectionMatrix() const noexcept
		{
			return _projectionMatrix;
		}

		Float4x4 CameraObject::getRotationMatrix() const noexcept
		{
			const float handnessSign = getHandednessSign();
			const Float3 kBaseForward = Float3::kAxisZ * handnessSign;
			const Float3& forwardDirectionXz = Float4x4::RotationMatrixY(_yaw) * kBaseForward;
			const Float3& leftDirection = Float3::CrossAndNormalize(forwardDirectionXz, Float3::kAxisY) * handnessSign;
			_forwardDirection = Float4x4::RotationMatrixAxisAngle(leftDirection * -handnessSign, _pitch) * forwardDirectionXz;
			const Float3& upDirection = Float3::CrossAndNormalize(leftDirection, _forwardDirection) * handnessSign;
			return Float4x4::RotationMatrixFromAxes(-leftDirection, upDirection, _forwardDirection * handnessSign);
		}

		float CameraObject::getHandednessSign() const noexcept
		{
			return (_isRightHanded ? -1.0f : +1.0f);
		}
	}
}
