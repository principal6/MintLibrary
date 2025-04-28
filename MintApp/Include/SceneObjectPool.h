#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_POOL_H_
#define _MINT_APP_SCENE_OBJECT_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/SharedPtr.h>


namespace mint
{
	class SceneObject;
	class SceneObjectComponent;
}

namespace mint
{
	class SceneObjectPool final
	{
	public:
		SceneObjectPool();
		~SceneObjectPool();

	public:
		SharedPtr<SceneObject> CreateSceneObject();

	public:
		template<typename ComponentType>
		ComponentType* CreateSceneObjectComponent();

	public:
		void UpdateScreenSize(const Float2& screenSize);

	public:
		const Vector<SceneObjectComponent*>& GetMeshComponents() const noexcept;
		const Vector<SceneObjectComponent*>& GetMesh2DComponents() const noexcept;

	public:
		uint32 GetSceneObjectCount() const noexcept;

	private:
		SharedPtr<SceneObject> CreateSceneObjectInternal(SharedPtr<SceneObject>&& sceneObject);
		void DestroyObjects();
		void DestroySceneObjectComponents(SceneObject& sceneObject);
		void DeregisterComponent(Vector<SceneObjectComponent*>& components, SceneObjectComponent* const component);

	private:
		Vector<SharedPtr<SceneObject>> _sceneObjects;

	private:
		Vector<SceneObjectComponent*> _meshComponents;
		Vector<SceneObjectComponent*> _mesh2DComponents;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_POOL_H_
