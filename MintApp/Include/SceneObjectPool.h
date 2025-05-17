#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_POOL_H_
#define _MINT_APP_SCENE_OBJECT_POOL_H_


#include <MintECS/Include/EntityPool.h>
#include <MintApp/Include/SceneObject.h>


namespace mint
{
	class SceneObjectPool final : public ECS::EntityPool<SceneObject>
	{
	public:
		SceneObjectPool();
		virtual ~SceneObjectPool();

	public:
		SceneObject CreateSceneObject() { return CreateEntity(); }
		void DestroySceneObject(SceneObject sceneObject) { DestroyEntity(sceneObject); }
		Transform& GetTransform(const SceneObject& sceneObject);

	protected:
		virtual void OnEntityCreated(SceneObject entity) override final;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_POOL_H_
