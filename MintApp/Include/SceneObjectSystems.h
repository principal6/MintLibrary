#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_SYSTEMS_H_
#define _MINT_APP_SCENE_OBJECT_SYSTEMS_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>


namespace mint
{
	class Float2;
	class SceneObject;
	enum class CameraMoveDirection : uint8;
	struct CameraComponent;
	struct TransformComponent;
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
		SceneObjectCameraSystem(SceneObjectRegistry& sceneObjectRegistry)
			: _sceneObjectRegistry(sceneObjectRegistry)
		{
			__noop;
		}
		~SceneObjectCameraSystem() = default;

	public:
		static void UpdateProjectionMatrix(CameraComponent& cameraComponent);

	public:
		void UpdateSystem(const Vector<SceneObject>& sceneObjects);
		void UpdateScreenSize(const Float2& screenSize);

	public:
		void DefaultSteerActiveCamera(const float deltaTime, const InputContext& inputContext, const bool is3DMode);
		void MoveActiveCamera(const CameraMoveDirection& cameraMoveDirection, float deltaTime);
		void RotateActiveCameraByMouseDelta(const Float2& mouseDelta);

	public:
		void SetActiveCameraObject(const SceneObject& cameraObject) { _activeCameraObject = cameraObject; }
		SceneObject GetActiveCameraObject() const { return _activeCameraObject; }
		Float4x4 MakeViewMatrix(const SceneObject& cameraObject) const;
		void IncreaseActiveCameraMoveSpeed();
		void DecreaseActiveCameraMoveSpeed();

	private:
		static void UpdateScreenSize(CameraComponent& cameraComponent, const Float2& screenSize);
		float GetHandednessSign(const CameraComponent& cameraComponent) const noexcept;
		CameraMoveSpeed GetFasterMoveSpeed(const CameraMoveSpeed moveSpeed) const;
		CameraMoveSpeed GetSlowerMoveSpeed(const CameraMoveSpeed moveSpeed) const;

	private:
		SceneObjectRegistry& _sceneObjectRegistry;
		SceneObject _activeCameraObject;
	};

	class SceneObjectSystems
	{
	public:
		SceneObjectSystems(SceneObjectRegistry& sceneObjectRegistry)
			: _sceneObjectRegistry(sceneObjectRegistry)
			, _cameraSystem(_sceneObjectRegistry)
		{
			__noop;
		}
		~SceneObjectSystems() = default;

	public:
		SceneObjectCameraSystem& GetCameraSystem() { return _cameraSystem; }

	private:
		SceneObjectRegistry& _sceneObjectRegistry;
		SceneObjectCameraSystem _cameraSystem;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_SYSTEMS_H_
