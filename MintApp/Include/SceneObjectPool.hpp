#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_POOL_HPP_
#define _MINT_APP_SCENE_OBJECT_POOL_HPP_


#include <MintApp/Include/SceneObjectPool.h>

#include <MintECS/Include/EntityRegistry.hpp>


namespace mint
{
	inline SceneObjectPool::SceneObjectPool()
	{
		__noop;
	}

	inline SceneObjectPool::~SceneObjectPool()
	{
		__noop;
	}

	inline Transform& SceneObjectPool::GetTransform(const SceneObject& sceneObject)
	{
		return GetComponent<TransformComponent>(sceneObject)->_transform;
	}

	inline void SceneObjectPool::OnEntityCreated(SceneObject entity)
	{
		// 모든 SceneObject 는 TransformComponent 를 가진다!
		AttachComponent(entity, TransformComponent());
	}
}


#endif // !_MINT_APP_SCENE_OBJECT_POOL_HPP_
