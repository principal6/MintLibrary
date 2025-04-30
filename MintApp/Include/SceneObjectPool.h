#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_POOL_H_
#define _MINT_APP_SCENE_OBJECT_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintContainer/Include/SerialAndIndex.h>

#include <MintApp/Include/SceneObject.h>


namespace mint
{
	class SceneObjectPool final
	{
	public:
		SceneObjectPool();
		~SceneObjectPool();

	public:
		SceneObject CreateSceneObject();
		void DestroySceneObject(SceneObject sceneObject);
		template<typename ComponentType>
		void AttachComponent(const SceneObject& sceneObject, const ComponentType& component);
		template<typename ComponentType>
		void AttachComponent(const SceneObject& sceneObject, ComponentType&& component);
		template<typename ComponentType>
		ComponentType* GetComponent(const SceneObject& sceneObject);
		template<typename ComponentType>
		ComponentType& GetComponentMust(const SceneObject& sceneObject);
		Transform& GetTransform(const SceneObject& sceneObject);

	public:
		const Vector<SceneObject>& GetSceneObjects() const noexcept;
		uint32 GetSceneObjectCount() const noexcept;

	private:
		Vector<SceneObject> _sceneObjects;
		uint32 _nextEmptySceneObjectIndex{ 0 };
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_POOL_H_
