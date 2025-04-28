#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_H_
#define _MINT_APP_SCENE_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float4x4.h>

#include <MintApp/Include/SceneObjectComponent.h>


namespace mint
{
	class SceneObjectComponent;
	class TransformComponent;
	class SceneObjectPool;
	enum class SceneObjectComponentType;
	struct Transform;
}

namespace mint
{
	class SceneObject
	{
		friend SceneObjectPool;

	public:
		virtual ~SceneObject();

	private:
		SceneObject(const SceneObjectPool* const sceneObjectPool);

	public:
		void AttachComponent(SceneObjectComponent* const component);
		void DetachComponent(SceneObjectComponent* const component);

	public:
		uint32 GetComponentCount() const noexcept;
		SceneObjectComponent* GetComponent(const SceneObjectComponentType type) const noexcept;

	public:
		void SetObjectTransform(const Transform& transform) noexcept;
		Transform& GetObjectTransform() noexcept;
		const Transform& GetObjectTransform() const noexcept;
		Float4x4 GetObjectTransformMatrix() const noexcept;

	protected:
		TransformComponent* GetObjectTransformComponent() const noexcept;

	protected:
		const SceneObjectPool* const _sceneObjectPool;

	protected:
		Vector<SceneObjectComponent*> _componentArray;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_H_
