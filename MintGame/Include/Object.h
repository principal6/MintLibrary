#pragma once


#ifndef _MINT_GAME_OBJECT_H_
#define _MINT_GAME_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float4x4.h>

#include <MintGame/Include/ObjectComponent.h>


namespace mint
{
	struct Transform;

	namespace Game
	{
		class ObjectComponent;
		class TransformComponent;
		class ObjectPool;
		enum class ObjectComponentType;
	}
}

namespace mint
{
	namespace Game
	{
		enum class ObjectType
		{
			INVALID,
			Object,
			CameraObject
		};

		class Object
		{
			friend ObjectPool;

		public:
			virtual ~Object();

		private:
			Object(const ObjectPool* const objectPool);

		protected:
			Object(const ObjectPool* const objectPool, const ObjectType objectType);

		public:
			void AttachComponent(ObjectComponent* const objectComponent);
			void DetachComponent(ObjectComponent* const objectComponent);

		public:
			ObjectType GetType() const noexcept;
			bool IsTypeOf(const ObjectType objectType) const noexcept;
			uint32 GetComponentCount() const noexcept;
			ObjectComponent* GetComponent(const ObjectComponentType type) const noexcept;

		public:
			void SetObjectTransform(const Transform& transform) noexcept;
			Transform& GetObjectTransform() noexcept;
			const Transform& GetObjectTransform() const noexcept;
			Float4x4 GetObjectTransformMatrix() const noexcept;

		protected:
			TransformComponent* GetObjectTransformComponent() const noexcept;
			float GetDeltaTimeSec() const noexcept;

		protected:
			const ObjectPool* const _objectPool;
			const ObjectType _objectType;

		protected:
			Vector<ObjectComponent*> _componentArray;
		};

	}
}


#include <MintGame/Include/Object.inl>


#endif // !_MINT_GAME_OBJECT_H_
