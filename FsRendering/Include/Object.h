#pragma once


#ifndef FS_OBJECT_H
#define FS_OBJECT_H


#include <CommonDefinitions.h>

#include <FsMath/Include/Float4x4.h>


namespace fs
{
	namespace Rendering
	{
		class ObjectComponent;
		class TransformComponent;
		class ObjectManager;
		enum class ObjectComponentType;
		struct Srt;


		class Object final
		{
			friend ObjectManager;

		private:
											Object();
		
		public:
											~Object();

		public:
			void							attachComponent(ObjectComponent* const objectComponent);
			void							detachComponent(ObjectComponent* const objectComponent);
		
		public:
			const uint32					getComponentCount() const noexcept;
			ObjectComponent*				getComponent(const ObjectComponentType type) const noexcept;
			fs::Rendering::Srt&				getObjectTransformSrt() noexcept;
			fs::Float4x4					getObjectTransformMatrix() const noexcept;

		private:
			TransformComponent*				getObjectTransformComponent() const noexcept;

		private:
			static ObjectManager*			_objectManager;

		private:
			std::vector<ObjectComponent*>	_componentArray;
		};

	}
}


#endif // !FS_OBJECT_H
