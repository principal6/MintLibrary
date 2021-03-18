#include <stdafx.h>
#include <FsRendering/Include/CameraObject.h>


namespace fs
{
	namespace Rendering
	{
		CameraObject::CameraObject()
			: Object(ObjectType::CameraObject)
			, _baseUpDirection{ 0.0f, 1.0f, 0.0f }
			, _baseForwardDirection{ 0.0f, 0.0f, 1.0f }
			, _focusOffset{ 0.0f, 0.0f, 2.0f }
			, _fov{ fs::Math::toRadian(60.0f) }
			, _nearZ{ 0.1f }
			, _farZ{ 1000.0f }
			, _screenRatio{ 1.0f }
			, _pitch{ 0.0f }
			, _yaw{ 0.0f }
			, _roll{ 0.0f }
			, _movementFactor{ 0.005f }
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
			_projectionMatrix = fs::Float4x4::projectionMatrixPerspective(_fov, _nearZ, _farZ, _screenRatio);
		}

		void CameraObject::move(const MoveDirection moveDirection)
		{
			const fs::Float3& rightDirection = fs::Float3::cross(_baseUpDirection, _forwardDirectionFinal);
			const fs::Float3& upDirection = fs::Float3::cross(_forwardDirectionFinal, rightDirection);

			fs::Rendering::Srt& srt = getObjectTransformSrt();
			switch (moveDirection)
			{
			case fs::Rendering::CameraObject::MoveDirection::Forward:
				srt._translation += _forwardDirectionFinal * _movementFactor;
				break;
			case fs::Rendering::CameraObject::MoveDirection::Backward:
				srt._translation -= _forwardDirectionFinal * _movementFactor;
				break;
			case fs::Rendering::CameraObject::MoveDirection::Leftward:
				srt._translation -= rightDirection * _movementFactor;
				break;
			case fs::Rendering::CameraObject::MoveDirection::Rightward:
				srt._translation += rightDirection * _movementFactor;
				break;
			case fs::Rendering::CameraObject::MoveDirection::Upward:
				srt._translation += upDirection * _movementFactor;
				break;
			case fs::Rendering::CameraObject::MoveDirection::Downward:
				srt._translation -= upDirection * _movementFactor;
				break;
			default:
				break;
			}
		}

		void CameraObject::rotatePitch(const float angle)
		{
			_pitch += angle * _rotationFactor;
			_pitch = fs::Math::limitAngleToPositiveNegativeTwoPiRotation(_pitch);
		}

		void CameraObject::rotateYaw(const float angle)
		{
			_yaw += angle * _rotationFactor;
			_yaw = fs::Math::limitAngleToPositiveNegativeTwoPiRotation(_yaw);
		}

		fs::Float4x4 CameraObject::getViewMatrix() const noexcept
		{
			const fs::Float4x4& rotationMatrix = getRotationMatrix();
			return rotationMatrix.transpose() * fs::Float4x4::translationMatrix(-getObjectTransformSrt()._translation);
		}

		const fs::Float4x4& CameraObject::getProjectionMatrix() const noexcept
		{
			return _projectionMatrix;
		}

		fs::Float4x4 CameraObject::getRotationMatrix() const noexcept
		{
			const fs::Float3& forwardDirectionAfterYaw = fs::Float4x4::rotationMatrixY(_yaw) * _baseForwardDirection;
			const fs::Float3& rightDirection = fs::Float3::crossAndNormalize(_baseUpDirection, forwardDirectionAfterYaw);
			_forwardDirectionFinal = fs::Float4x4::rotationMatrixAxisAngle(rightDirection, _pitch) * forwardDirectionAfterYaw;
			const fs::Float3& upDirection = fs::Float3::crossAndNormalize(_forwardDirectionFinal, rightDirection);
			return fs::Float4x4::rotationMatrixFromAxes(rightDirection, upDirection, _forwardDirectionFinal);
		}
	}
}
