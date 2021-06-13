#include <stdafx.h>
#include <MintRendering/Include/CameraObject.h>
#include <MintRendering/Include/ObjectPool.hpp>


namespace mint
{
    namespace Rendering
    {
        CameraObject::CameraObject(const ObjectPool* const objectPool)
            : Object(objectPool, ObjectType::CameraObject)
            , _baseUpDirection{ 0.0f, 1.0f, 0.0f }
            , _baseForwardDirection{ 0.0f, 0.0f, 1.0f }
            , _focusOffset{ 0.0f, 0.0f, 2.0f }
            , _fov{ mint::Math::toRadian(60.0f) }
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
            _projectionMatrix = mint::Float4x4::projectionMatrixPerspective(_fov, _nearZ, _farZ, _screenRatio);
        }

        void CameraObject::move(const MoveDirection moveDirection)
        {
            const float deltaTimeS = getDeltaTimeS();

            const mint::Float3& rightDirection = mint::Float3::cross(_baseUpDirection, _forwardDirectionFinal);
            const mint::Float3& upDirection = mint::Float3::cross(_forwardDirectionFinal, rightDirection);
            
            const float moveSpeedFloat = getMoveSpeedAsFloat((true == _isBoostMode) ? getFasterMoveSpeed(getFasterMoveSpeed(_moveSpeed)) : _moveSpeed);
            mint::Rendering::Srt& srt = getObjectTransformSrt();
            switch (moveDirection)
            {
            case mint::Rendering::CameraObject::MoveDirection::Forward:
                srt._translation += _forwardDirectionFinal * moveSpeedFloat * deltaTimeS;
                break;
            case mint::Rendering::CameraObject::MoveDirection::Backward:
                srt._translation -= _forwardDirectionFinal * moveSpeedFloat * deltaTimeS;
                break;
            case mint::Rendering::CameraObject::MoveDirection::Leftward:
                srt._translation -= rightDirection * moveSpeedFloat * deltaTimeS;
                break;
            case mint::Rendering::CameraObject::MoveDirection::Rightward:
                srt._translation += rightDirection * moveSpeedFloat * deltaTimeS;
                break;
            case mint::Rendering::CameraObject::MoveDirection::Upward:
                srt._translation += upDirection * moveSpeedFloat * deltaTimeS;
                break;
            case mint::Rendering::CameraObject::MoveDirection::Downward:
                srt._translation -= upDirection * moveSpeedFloat * deltaTimeS;
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

        void CameraObject::setIsBoostMode(const bool isBoostMode) noexcept
        {
            _isBoostMode = isBoostMode;
        }

        void CameraObject::rotatePitch(const float angle)
        {
            _pitch += angle * _rotationFactor;
            _pitch = mint::Math::limitAngleToPositiveNegativeTwoPiRotation(_pitch);
        }

        void CameraObject::rotateYaw(const float angle)
        {
            _yaw += angle * _rotationFactor;
            _yaw = mint::Math::limitAngleToPositiveNegativeTwoPiRotation(_yaw);
        }

        mint::Float4x4 CameraObject::getViewMatrix() const noexcept
        {
            const mint::Float4x4& rotationMatrix = getRotationMatrix();
            return rotationMatrix.transpose() * mint::Float4x4::translationMatrix(-getObjectTransformSrt()._translation);
        }

        const mint::Float4x4& CameraObject::getProjectionMatrix() const noexcept
        {
            return _projectionMatrix;
        }

        mint::Float4x4 CameraObject::getRotationMatrix() const noexcept
        {
            const mint::Float3& forwardDirectionAfterYaw = mint::Float4x4::rotationMatrixY(_yaw) * _baseForwardDirection;
            const mint::Float3& rightDirection = mint::Float3::crossAndNormalize(_baseUpDirection, forwardDirectionAfterYaw);
            _forwardDirectionFinal = mint::Float4x4::rotationMatrixAxisAngle(rightDirection, _pitch) * forwardDirectionAfterYaw;
            const mint::Float3& upDirection = mint::Float3::crossAndNormalize(_forwardDirectionFinal, rightDirection);
            return mint::Float4x4::rotationMatrixFromAxes(rightDirection, upDirection, _forwardDirectionFinal);
        }
    }
}
