#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_POOL_HPP_
#define _MINT_APP_SCENE_OBJECT_POOL_HPP_


#include <MintApp/Include/SceneObjectPool.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintApp/Include/DeltaTimer.h>
#include <MintApp/Include/SceneObject.h>


namespace mint
{
	inline SceneObjectPool::SceneObjectPool()
	{
		__noop;
	}

	inline SceneObjectPool::~SceneObjectPool()
	{
		DestroyObjects();
	}

	MINT_INLINE SharedPtr<SceneObject> SceneObjectPool::CreateSceneObject()
	{
		return CreateSceneObjectInternal(MakeShared<SceneObject>(SceneObject(this)));
	}

	MINT_INLINE void SceneObjectPool::DestroyObjects()
	{
		const uint32 sceneObjectCount = GetSceneObjectCount();
		for (uint32 sceneObjectIndex = 0; sceneObjectIndex < sceneObjectCount; ++sceneObjectIndex)
		{
			if (_sceneObjects[sceneObjectIndex].IsValid())
			{
				DestroySceneObjectComponents(*_sceneObjects[sceneObjectIndex]);
			}
		}
		_sceneObjects.Clear();
	}

	MINT_INLINE SharedPtr<SceneObject> SceneObjectPool::CreateSceneObjectInternal(SharedPtr<SceneObject>&& sceneObject)
	{
		_sceneObjects.PushBack(sceneObject);
		sceneObject->AttachComponent(MINT_NEW(TransformComponent)); // 모든 SceneObject는 TransformComponent 를 필수로 가집니다.
		return _sceneObjects.Back();
	}

	template<typename ComponentType>
	MINT_INLINE ComponentType* SceneObjectPool::CreateSceneObjectComponent()
	{
		ComponentType* component = MINT_NEW(ComponentType);
		const SceneObjectComponentType type = component->GetType();
		if (type == SceneObjectComponentType::MeshComponent)
		{
			_meshComponents.PushBack(component);
		}
		else if (type == SceneObjectComponentType::Mesh2DComponent)
		{
			_mesh2DComponents.PushBack(component);
		}
		return component;
	}

	MINT_INLINE void SceneObjectPool::DestroySceneObjectComponents(SceneObject& sceneObject)
	{
		const uint32 componentCount = static_cast<uint32>(sceneObject._componentArray.Size());
		for (uint32 componentIndex = 0; componentIndex < componentCount; ++componentIndex)
		{
			SceneObjectComponent*& component = sceneObject._componentArray[componentIndex];
			if (component != nullptr)
			{
				const SceneObjectComponentType componentType = component->GetType();
				if (componentType == SceneObjectComponentType::MeshComponent)
				{
					DeregisterComponent(_meshComponents, component);
				}
				else if (componentType == SceneObjectComponentType::Mesh2DComponent)
				{
					DeregisterComponent(_mesh2DComponents, component);
				}

				MINT_DELETE(component);
			}
		}

		sceneObject._componentArray.Clear();
	}

	MINT_INLINE void SceneObjectPool::DeregisterComponent(Vector<SceneObjectComponent*>& components, SceneObjectComponent* const component)
	{
		if (component == nullptr)
		{
			return;
		}

		int32 foundIndex = -1;
		const int32 componentCount = static_cast<int32>(components.Size());
		for (int32 i = 0; i < componentCount; ++i)
		{
			if (components[i]->GetID() == component->GetID())
			{
				foundIndex = i;
				break;
			}
		}

		if (foundIndex >= 0)
		{
			if (foundIndex < componentCount)
			{
				std::swap(components[foundIndex], components.Back());
			}
			components.PopBack();
		}
	}

	MINT_INLINE void SceneObjectPool::UpdateScreenSize(const Float2& screenSize)
	{
		const uint32 sceneObjectCount = _sceneObjects.Size();
		for (uint32 sceneObjectIndex = 0; sceneObjectIndex < sceneObjectCount; ++sceneObjectIndex)
		{
			SharedPtr<SceneObject>& sceneObject = _sceneObjects[sceneObjectIndex];
			CameraComponent* const cameraComponent = static_cast<CameraComponent*>(sceneObject->GetComponent(SceneObjectComponentType::CameraComponent));
			if (cameraComponent != nullptr)
			{
				cameraComponent->UpdateScreenSize(screenSize);
			}
		}
	}

	MINT_INLINE const Vector<SceneObjectComponent*>& SceneObjectPool::GetMeshComponents() const noexcept
	{
		return _meshComponents;
	}

	MINT_INLINE const Vector<SceneObjectComponent*>& SceneObjectPool::GetMesh2DComponents() const noexcept
	{
		return _mesh2DComponents;
	}

	MINT_INLINE uint32 SceneObjectPool::GetSceneObjectCount() const noexcept
	{
		return _sceneObjects.Size();
	}
}


#endif // !_MINT_APP_SCENE_OBJECT_POOL_HPP_
