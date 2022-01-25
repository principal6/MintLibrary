#include <stdafx.h>
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

        void CameraObject::steer(Platform::InputContext& inputContext, const bool isMoveLocked)
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
            const float deltaTimeS = getDeltaTimeS();

            const float handnessSign = getHandednessSign();
            const Float3& leftDirection = Float3::cross(_forwardDirection, Float3::kAxisY) * handnessSign;
            const Float3& upDirection = Float3::cross(leftDirection, _forwardDirection) * handnessSign;
            
            const float moveSpeedFloat = getMoveSpeedAsFloat((_isBoostMode) ? getFasterMoveSpeed(getFasterMoveSpeed(_moveSpeed)) : _moveSpeed);
            Transform& transform = getObjectTransform();
            switch (moveDirection)
            {
            case CameraObject::MoveDirection::Forward:
                transform._translation += _forwardDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Backward:
                transform._translation -= _forwardDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Leftward:
                transform._translation += leftDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Rightward:
                transform._translation -= leftDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Upward:
                transform._translation += upDirection * moveSpeedFloat * deltaTimeS;
                break;
            case CameraObject::MoveDirection::Downward:
                transform._translation -= upDirection * moveSpeedFloat * deltaTimeS;
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
            return rotationMatrix.transpose() * Float4x4::translationMatrix(-getObjectTransform()._translation);
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
