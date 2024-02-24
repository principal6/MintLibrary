#pragma once


#ifndef _MINT_GAME_OBJECT_H_
#define _MINT_GAME_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float4x4.h>

#include <MintApp/Include/ObjectComponent.h>


namespace mint
{
	class ObjectComponent;
	class TransformComponent;
	class ObjectPool;
	enum class ObjectComponentType;
	struct Transform;
}

namespace mint
{
	class Object
	{
		friend ObjectPool;

	public:
		virtual ~Object();

	private:
		Object(const ObjectPool* const objectPool);

	public:
		void AttachComponent(ObjectComponent* const objectComponent);
		void DetachComponent(ObjectComponent* const objectComponent);

	public:
		uint32 GetComponentCount() const noexcept;
		ObjectComponent* GetComponent(const ObjectComponentType type) const noexcept;

	public:
		void SetObjectTransform(const Transform& transform) noexcept;
		Transform& GetObjectTransform() noexcept;
		const Transform& GetObjectTransform() const noexcept;
		Float4x4 GetObjectTransformMatrix() const noexcept;

	protected:
		TransformComponent* GetObjectTransformComponent() const noexcept;

	protected:
		const ObjectPool* const _objectPool;

	protected:
		Vector<ObjectComponent*> _componentArray;
	};
}


#endif // !_MINT_GAME_OBJECT_H_
