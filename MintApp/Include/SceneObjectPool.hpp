#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_POOL_HPP_
#define _MINT_APP_SCENE_OBJECT_POOL_HPP_


#include <MintApp/Include/SceneObjectPool.h>

//#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintApp/Include/DeltaTimer.h>
#include <MintApp/Include/SceneObject.hpp>


namespace mint
{
	inline SceneObjectPool::SceneObjectPool()
	{
		_nextEmptySceneObjectIndex = 0;
	}

	inline SceneObjectPool::~SceneObjectPool()
	{
		// TODO: Destroy 로직???
		// TODO: Destroy 로직???
		// TODO: Destroy 로직???
	}

	MINT_INLINE SceneObject SceneObjectPool::CreateSceneObject()
	{
		if (_nextEmptySceneObjectIndex >= _sceneObjects.Size())
		{
			if (_sceneObjects.IsEmpty() == true)
			{
				_sceneObjects.Resize(16);
			}
			else
			{
				_sceneObjects.Resize(_sceneObjects.Capacity() * 2);
			}
		}

		SceneObject& sceneObject = _sceneObjects[_nextEmptySceneObjectIndex];
		sceneObject.SetSerialAndIndex(sceneObject.GetSerial() + 1, _nextEmptySceneObjectIndex);
		
		const uint32 nextEmptySceneObjectIndexCache = _nextEmptySceneObjectIndex;
		const uint32 poolSize = _sceneObjects.Size();
		for (uint32 i = _nextEmptySceneObjectIndex + 1; i < poolSize; ++i)
		{
			if (_sceneObjects[i].IsValid() == false)
			{
				_nextEmptySceneObjectIndex = i;
				break;
			}
		}
		if (nextEmptySceneObjectIndexCache == _nextEmptySceneObjectIndex)
		{
			_nextEmptySceneObjectIndex = _sceneObjects.Size();
		}

		// 모든 SceneObject 는 TransformComponent 를 가진다!
		AttachComponent(sceneObject, TransformComponent());

		return sceneObject;
	}

	inline void SceneObjectPool::DestroySceneObject(SceneObject sceneObject)
	{
		const uint32 index = static_cast<uint32>(sceneObject.GetIndex());
		MINT_ASSERT(index < _sceneObjects.Size(), "로직 상 반드시 보장되어야 합니다!");
		_sceneObjects[index].SetInvalid();
		_nextEmptySceneObjectIndex = Min(index, _nextEmptySceneObjectIndex);

		const Vector<ISceneObjectComponentPool*>& componentPools = SceneObjectComponentPoolRegistry::GetInstance().GetComponentPools();
		for (ISceneObjectComponentPool* const componentPool : componentPools)
		{
			componentPool->RemoveComponentFrom(sceneObject);
		}
	}

	// TODO: CameraComponent Pool 만 돌면 된다!!
	// TODO: CameraComponent Pool 만 돌면 된다!!
	// TODO: CameraComponent Pool 만 돌면 된다!!
	//MINT_INLINE void SceneObjectPool::UpdateScreenSize(const Float2& screenSize)
	//{
	//	const uint32 sceneObjectCount = _sceneObjects.Size();
	//	for (uint32 sceneObjectIndex = 0; sceneObjectIndex < sceneObjectCount; ++sceneObjectIndex)
	//	{
	//		SharedPtr<SceneObject>& sceneObject = _sceneObjects[sceneObjectIndex];
	//		CameraComponent* const cameraComponent = static_cast<CameraComponent*>(sceneObject->GetComponent(SceneObjectComponentType::CameraComponent));
	//		if (cameraComponent != nullptr)
	//		{
	//			cameraComponent->UpdateScreenSize(screenSize);
	//		}
	//	}
	//}

	inline const Vector<SceneObject>& SceneObjectPool::GetSceneObjects() const noexcept
	{
		return _sceneObjects;
	}

	MINT_INLINE uint32 SceneObjectPool::GetSceneObjectCount() const noexcept
	{
		return _sceneObjects.Size();
	}

	template<typename ComponentType>
	inline void SceneObjectPool::AttachComponent(const SceneObject& sceneObject, const ComponentType& component)
	{
		return SceneObjectComponentPool<ComponentType>::GetInstance().AddComponentTo(sceneObject, component);
	}

	template<typename ComponentType>
	inline void SceneObjectPool::AttachComponent(const SceneObject& sceneObject, ComponentType&& component)
	{
		return SceneObjectComponentPool<ComponentType>::GetInstance().AddComponentTo(sceneObject, std::move(component));
	}

	template<typename ComponentType>
	inline ComponentType* SceneObjectPool::GetComponent(const SceneObject& sceneObject)
	{
		return SceneObjectComponentPool<ComponentType>::GetInstance().GetComponent(sceneObject);
	}
}


#endif // !_MINT_APP_SCENE_OBJECT_POOL_HPP_
