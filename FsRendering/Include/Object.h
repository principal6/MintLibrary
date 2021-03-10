#pragma once


#ifndef FS_OBJECT_H
#define FS_OBJECT_H


#include <CommonDefinitions.h>

#include <FsMath/Include/Float4x4.h>
#include <FsRendering/Include/ObjectComponent.h>


namespace fs
{
	namespace Rendering
	{
		class ObjectComponent;
		class TransformComponent;
		class ObjectManager;
		enum class ObjectComponentType;
		struct Srt;

		enum class ObjectType
		{
			INVALID,
			Object,
			CameraObject
		};


		class Object
		{
			friend ObjectManager;

		private:
											Object();

		protected:
											Object(const ObjectType objectType);
			virtual							~Object();
		
		public:
			void							attachComponent(ObjectComponent* const objectComponent);
			void							detachComponent(ObjectComponent* const objectComponent);
		
		public:
			const ObjectType				getType() const noexcept;
			const bool						isTypeOf(const ObjectType objectType) const noexcept;
			const uint32					getComponentCount() const noexcept;
			ObjectComponent*				getComponent(const ObjectComponentType type) const noexcept;
			fs::Rendering::Srt&				getObjectTransformSrt() noexcept;
			const fs::Rendering::Srt&		getObjectTransformSrt() const noexcept;
			fs::Float4x4					getObjectTransformMatrix() const noexcept;

		protected:
			TransformComponent*				getObjectTransformComponent() const noexcept;

		protected:
			const ObjectType				_objectType;

		protected:
			std::vector<ObjectComponent*>	_componentArray;
		};

	}
}


#include <FsRendering/Include/Object.inl>


#endif // !FS_OBJECT_H
