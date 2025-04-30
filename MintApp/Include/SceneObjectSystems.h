#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_SYSTEMS_H_
#define _MINT_APP_SCENE_OBJECT_SYSTEMS_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>


namespace mint
{
	class Float2;
	class SceneObject;
	struct CameraComponent;
	struct TransformComponent;
	enum class CameraMoveDirection : uint8;
}

namespace mint
{
	class SceneObjectComponentHelpers
	{
	public:
		static void MakePerspectiveCamera(float fov, float nearZ, float farZ, float screenRatio, CameraComponent& cameraComponent);
		static void MakeOrthographic2DCamera(const Float2& screenSize, CameraComponent& cameraComponent);
		static void RotateCameraPitch(float deltaPitch, CameraComponent& cameraComponent);
		static void RotateCameraYaw(float deltaYaw, CameraComponent& cameraComponent);
	};

	class SceneObjectCameraSystem
	{
	public:
		SceneObjectCameraSystem() = default;
		~SceneObjectCameraSystem() = default;

	public:
		static void UpdateProjectionMatrix(CameraComponent& cameraComponent);

	public:
		void UpdateSystem(const Vector<SceneObject>& sceneObjects);
		void UpdateScreenSize(const Float2& screenSize);

	public:
		void DefaultSteerCurrentCamera(const float deltaTime, const InputContext& inputContext, const bool is3DMode);
		void MoveCurrentCamera(const CameraMoveDirection& cameraMoveDirection, float deltaTime);
		void RotateCurrentCameraByMouseDelta(const Float2& mouseDelta);

	public:
		void SetCurrentCameraObject(const SceneObject& cameraObject) { _currentCameraObject = cameraObject; }
		SceneObject GetCurrentCameraObject() const { return _currentCameraObject; }
		Float4x4 MakeViewMatrix(const SceneObject& cameraObject) const;
		void IncreaseCurrentCameraMoveSpeed();
		void DecreaseCurrentCameraMoveSpeed();

	private:
		static void UpdateScreenSize(CameraComponent& cameraComponent, const Float2& screenSize);
		float GetHandednessSign(const CameraComponent& cameraComponent) const noexcept;
		CameraMoveSpeed GetFasterMoveSpeed(const CameraMoveSpeed moveSpeed) const;
		CameraMoveSpeed GetSlowerMoveSpeed(const CameraMoveSpeed moveSpeed) const;

	private:
		SceneObject _currentCameraObject;
	};

	class SceneObjectSystems
	{
	public:
		SceneObjectSystems() = default;
		~SceneObjectSystems() = default;

	public:
		SceneObjectCameraSystem& GetCameraSystem() { return _cameraSystem; }

	private:
		SceneObjectCameraSystem _cameraSystem;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_SYSTEMS_H_
