#pragma once


#ifndef _MINT_GAME_CAMERA_OBJECT_H_
#define _MINT_GAME_CAMERA_OBJECT_H_


#include <MintGAME/Include/Object.h>


namespace mint
{
	class InputContext;

	namespace Game
	{
		class ObjectPool;
	}
}

namespace mint
{
	namespace Game
	{
		class CameraObject final : public Object
		{
			friend ObjectPool;

		public:
			enum class MoveDirection
			{
				Forward,
				Backward,
				Leftward,
				Rightward,
				Upward,
				Downward
			};

		private:
			enum class MoveSpeed : uint8
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

			MINT_INLINE constexpr float GetMoveSpeedAsFloat(const MoveSpeed moveSpeed)
			{
				static_assert(static_cast<uint8>(MoveSpeed::x0_125) == 0, "Base 가 달라졌습니다!");
				const uint8 moveSpeedUint8 = static_cast<uint8>(moveSpeed);
				return 0.125f * static_cast<float>(Math::Pow2_Uint32(moveSpeedUint8));
			}

			MINT_INLINE constexpr MoveSpeed GetFasterMoveSpeed(const MoveSpeed moveSpeed)
			{
				const uint8 moveSpeedUint8 = static_cast<uint8>(moveSpeed);
				const uint8 moveSpeedMax = static_cast<uint8>(MoveSpeed::COUNT) - 1;
				if (moveSpeedUint8 < moveSpeedMax)
				{
					return static_cast<MoveSpeed>(moveSpeedUint8 + 1);
				}
				return moveSpeed;
			}

			MINT_INLINE constexpr MoveSpeed GetSlowerMoveSpeed(const MoveSpeed moveSpeed)
			{
				const uint8 moveSpeedUint8 = static_cast<uint8>(moveSpeed);
				if (moveSpeedUint8 > 0)
				{
					return static_cast<MoveSpeed>(moveSpeedUint8 - 1);
				}
				return moveSpeed;
			}

		private:
			CameraObject(const ObjectPool* const objectPool);

		public:
			virtual ~CameraObject();

		public:
			void SetPerspectiveFov(const float fov);
			void SetPerspectiveZRange(const float nearZ, const float farZ);
			void SetPerspectiveScreenRatio(const float screenRatio);

		public:
			void Steer(const InputContext& inputContext, const bool isMoveLocked);
			void Move(const MoveDirection moveDirection);
			void RotatePitch(const float angle);
			void RotateYaw(const float angle);
			void RotateByMouseDelta(const Float2& mouseDelta);

		public:
			void IncreaseMoveSpeed() noexcept;
			void DecreaseMoveSpeed() noexcept;
			void SetBoostMode(const bool isBoostMode) noexcept;

		public:
			Float4x4 GetViewMatrix() const noexcept;
			const Float4x4& GetProjectionMatrix() const noexcept;

		private:
			void UpdatePerspectiveMatrix() noexcept;
			Float4x4 GetRotationMatrix() const noexcept;
			float GetHandednessSign() const noexcept;

		private:
			Float4x4 _projectionMatrix;

			mutable Float3 _forwardDirection;

			bool _isRightHanded;
			float _fov;
			float _nearZ;
			float _farZ;
			float _screenRatio;

			float _pitch;
			float _yaw;
			float _roll;
			MoveSpeed _moveSpeed;
			bool _isBoostMode;
		};
	}
}


#endif // !_MINT_GAME_CAMERA_OBJECT_H_
