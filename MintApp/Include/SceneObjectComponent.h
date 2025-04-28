#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_COMPONENT_H_
#define _MINT_APP_SCENE_OBJECT_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/ID.h>

#include <MintMath/Include/Transform.h>


namespace mint
{
	class SceneObject;
	class SceneObjectComponent;
	class InputContext;
}

namespace mint
{
	enum class SceneObjectComponentType
	{
		Invalid,
		TransformComponent,
		CameraComponent,
		MeshComponent,
		Mesh2DComponent,
		Collision2DComponent,
	};


	class SceneObjectComponentID final : public ID32
	{
		friend SceneObjectComponent;
	};


	class SceneObjectComponent abstract
	{
		friend SceneObject;

	private:
		static std::atomic<uint32> _nextRawID;

	public:
		SceneObjectComponent(const SceneObjectComponentType type);
		virtual ~SceneObjectComponent() { __noop; }

	public:
		MINT_INLINE SceneObjectComponentType GetType() const noexcept { return _type; }
		MINT_INLINE bool IsTypeOf(const SceneObjectComponentType type) const noexcept { return _type == type; }
		MINT_INLINE const SceneObjectComponentID& GetID() const noexcept { return _id; }
		MINT_INLINE SceneObject* GetOwnerObject() const noexcept { return _ownerObject; }

	protected:
		SceneObject* _ownerObject;
		SceneObjectComponentType _type;
		SceneObjectComponentID _id;
	};

	class TransformComponent : public SceneObjectComponent
	{
	public:
		TransformComponent() : SceneObjectComponent(SceneObjectComponentType::TransformComponent) { __noop; }
		virtual ~TransformComponent() { __noop; }

	public:
		Transform _transform;
	};

	class CameraComponent : public SceneObjectComponent
	{
	public:
		CameraComponent();
		virtual ~CameraComponent() { __noop; }

	public:
		void SetPerspectiveCamera(const float fov, const float nearZ, const float farZ, const float screenRatio);
		void SetOrthographic2DCamera(const Float2& screenSize);
		void UpdateScreenSize(const Float2& screenSize);

	public:
		void SteerDefault(const float deltaTime, const InputContext& inputContext, const bool is3DMode);
		enum class MoveDirection
		{
			Forward,
			Backward,
			Leftward,
			Rightward,
			Upward,
			Downward
		};
		void Move(const MoveDirection moveDirection, const float deltaTime);
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
		void UpdateProjectionMatrix() noexcept;
		Float4x4 GetRotationMatrix() const noexcept;
		float GetHandednessSign() const noexcept;

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
		Float4x4 _projectionMatrix;
		mutable Float3 _forwardDirection;
		bool _usePerspectiveProjection;
		bool _isRightHanded;
		float _fov;
		union
		{
			struct
			{
				float _nearZ;
				float _farZ;
			};
			Float2 _screenSize;
		};
		float _screenRatio;

		float _pitch;
		float _yaw;
		float _roll;
		MoveSpeed _moveSpeed;
		bool _isBoostMode;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_COMPONENT_H_
