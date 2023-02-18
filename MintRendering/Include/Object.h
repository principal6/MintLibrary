#pragma once


#ifndef _MINT_RENDERING_OBJECT_H_
#define _MINT_RENDERING_OBJECT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float4x4.h>

#include <MintRendering/Include/IObjectComponent.h>


namespace mint
{
	struct Transform;


	namespace Rendering
	{
		class IObjectComponent;
		class TransformComponent;
		class ObjectPool;
		enum class ObjectComponentType;

		enum class ObjectType
		{
			INVALID,
			Object,
			CameraObject
		};


		class Object
		{
			friend ObjectPool;

		private:
			Object(const ObjectPool* const objectPool);

		protected:
			Object(const ObjectPool* const objectPool, const ObjectType objectType);
			virtual ~Object();

		public:
			void AttachComponent(IObjectComponent* const objectComponent);
			void DetachComponent(IObjectComponent* const objectComponent);

		public:
			ObjectType GetType() const noexcept;
			bool IsTypeOf(const ObjectType objectType) const noexcept;
			uint32 GetComponentCount() const noexcept;
			IObjectComponent* GetComponent(const ObjectComponentType type) const noexcept;

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
			Vector<IObjectComponent*> _componentArray;
		};

	}
}


#include <MintRendering/Include/Object.inl>


#endif // !_MINT_RENDERING_OBJECT_H_
