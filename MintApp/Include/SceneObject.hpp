#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_HPP_
#define _MINT_APP_SCENE_OBJECT_HPP_


#include <MintApp/Include/SceneObject.h>

#include <MintContainer/Include/ContiguousHashMap.hpp>


namespace mint
{
	inline uint64 Hasher<SceneObject>::operator()(const SceneObject& value) const noexcept
	{
		return ComputeHash(value.GetRawValue());
	}

#pragma region SceneObjectComponentPool
	template<typename ComponentType>
	inline SceneObjectComponentPool<ComponentType>::SceneObjectComponentPool()
	{
	}

	template<typename ComponentType>
	inline SceneObjectComponentPool<ComponentType>::~SceneObjectComponentPool()
	{
		__noop;
	}

	template<typename ComponentType>
	inline SceneObjectComponentPool<ComponentType>& SceneObjectComponentPool<ComponentType>::GetInstance()
	{
		static SceneObjectComponentPool<ComponentType> sInstance;
		static bool isRegistered = false;
		if (isRegistered == false)
		{
			SceneObjectComponentPoolRegistry::GetInstance().RegisterComponentPool(sInstance);
			isRegistered = true;
		}
		return sInstance;
	}

	template<typename ComponentType>
	inline void SceneObjectComponentPool<ComponentType>::AddComponentTo(const SceneObject& sceneObject, const ComponentType& component)
	{
		_componentMap.Insert(sceneObject, component);
	}
	
	template<typename ComponentType>
	inline void SceneObjectComponentPool<ComponentType>::AddComponentTo(const SceneObject& sceneObject, ComponentType&& component)
	{
		_componentMap.Insert(sceneObject, std::move(component));
	}

	template<typename ComponentType>
	inline void SceneObjectComponentPool<ComponentType>::RemoveComponentFrom(const SceneObject& sceneObject)
	{
		_componentMap.Erase(sceneObject);
	}

	template<typename ComponentType>
	inline bool SceneObjectComponentPool<ComponentType>::HasComponent(const SceneObject& sceneObject) const
	{
		return _componentMap.Contains(sceneObject);
	}

	template<typename ComponentType>
	inline void SceneObjectComponentPool<ComponentType>::CopyComponent(const SceneObject& sourceEntity, const SceneObject& targetEntity)
	{
		const ComponentType* const sourceComponent = GetComponent(sourceEntity);
		AddComponentTo(targetEntity, ComponentType(*sourceComponent));
	}

	template<typename ComponentType>
	inline ComponentType* SceneObjectComponentPool<ComponentType>::GetComponent(const SceneObject& sceneObject)
	{
		return _componentMap.Find(sceneObject);
	}

	template<typename ComponentType>
	inline ContiguousHashMap<SceneObject, ComponentType>& SceneObjectComponentPool<ComponentType>::GetComponentMap()
	{
		return _componentMap;
	}
#pragma endregion
}


#endif // !_MINT_APP_SCENE_OBJECT_HPP_
