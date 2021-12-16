#include <stdafx.h>
#include <MintRendering/Include/CameraObject.h>
#include <MintRendering/Include/ObjectPool.hpp>


namespace mint
{
    namespace Rendering
    {
        CameraObject::CameraObject(const ObjectPool* const objectPool)
            : Object(objectPool, ObjectType::CameraObject)
            , _isRightHanded{ true }
            , _fov{ Math::toRadian(60.0f) }
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
            _projectionMatrix = Float4x4::projectionMatrixPerspectiveYUP(_isRightHanded, _fov, _nearZ, _farZ, _screenRatio);
        }

        void CameraObject::move(const MoveDirection moveDirection)
        {
            const float deltaTimeS = getDeltaTimeS();

            const float handnessSign = getHandednessSign();
            const Float3& leftDirection = Float3::cross(_forwardDirection, Float3::kAxisY) * handnessSign;
            const Float3& upDirection = Float3::cross(leftDirection, _forwardDirection) * handnessSign;
            
            const float moveSpeedFloat = getMoveSpeedAsFloat((true == _isBoostMode) ? getFasterMoveSpeed(getFasterMoveSpeed(_moveSpeed)) : _moveSpeed);
            SRT& srt = getObjectTransformSRT();
            switch (moveDirection)
            {
            case CameraObject::MoveDirection::Forward:
                srt._translation += _forwardDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Backward:
                srt._translation -= _forwardDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Leftward:
                srt._translation += leftDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Rightward:
                srt._translation -= leftDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Upward:
                srt._translation += upDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Downward:
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
            _pitch = Math::limitAngleToPositiveNegativeTwoPiRotation(_pitch);
        }

        void CameraObject::rotateYaw(const float angle)
        {
            _yaw += angle * _rotationFactor;
            _yaw = Math::limitAngleToPositiveNegativeTwoPiRotation(_yaw);
        }

        void CameraObject::rotateByMouseDelta(const Float2& mouseDelta)
        {
            const float handnessSign = getHandednessSign();
            rotatePitch(mouseDelta._y);
            rotateYaw(mouseDelta._x * handnessSign);
        }

        Float4x4 CameraObject::getViewMatrix() const noexcept
        {
            const Float4x4& rotationMatrix = getRotationMatrix();
            return rotationMatrix.transpose() * Float4x4::translationMatrix(-getObjectTransformSRT()._translation);
        }

        const Float4x4& CameraObject::getProjectionMatrix() const noexcept
        {
            return _projectionMatrix;
        }

        Float4x4 CameraObject::getRotationMatrix() const noexcept
        {
            const float handnessSign = getHandednessSign();
            const Float3 kBaseForward = Float3::kAxisZ * handnessSign;
            const Float3& forwardDirectionXz = Float4x4::rotationMatrixY(_yaw) * kBaseForward;
            const Float3& leftDirection = Float3::crossAndNormalize(forwardDirectionXz, Float3::kAxisY) * handnessSign;
            _forwardDirection = Float4x4::rotationMatrixAxisAngle(leftDirection * -handnessSign, _pitch) * forwardDirectionXz;
            const Float3& upDirection = Float3::crossAndNormalize(leftDirection, _forwardDirection) * handnessSign;
            return Float4x4::rotationMatrixFromAxes(-leftDirection, upDirection, _forwardDirection * handnessSign);
        }

        const float CameraObject::getHandednessSign() const noexcept
        {
            return (_isRightHanded ? -1.0f : +1.0f);
        }
    }
}
