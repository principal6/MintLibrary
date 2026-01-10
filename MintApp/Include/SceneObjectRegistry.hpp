#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_POOL_HPP_
#define _MINT_APP_SCENE_OBJECT_POOL_HPP_


#include <MintApp/Include/SceneObjectRegistry.h>

#include <MintECS/Include/EntityRegistry.hpp>


namespace mint
{
	inline SceneObjectRegistry::SceneObjectRegistry()
	{
		__noop;
	}

	inline SceneObjectRegistry::~SceneObjectRegistry()
	{
		__noop;
	}

	inline Transform& SceneObjectRegistry::GetTransform(const SceneObject& sceneObject)
	{
		return GetComponentMust<TransformComponent>(sceneObject)._transform;
	}

	inline void SceneObjectRegistry::OnEntityCreated(SceneObject entity)
	{
		// 모든 SceneObject 는 TransformComponent 를 가진다!
		AttachComponent(entity, TransformComponent());
	}
}


#endif // !_MINT_APP_SCENE_OBJECT_POOL_HPP_
