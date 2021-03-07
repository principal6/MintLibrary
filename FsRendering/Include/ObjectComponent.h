#pragma once


#ifndef FS_OBJECT_COMPONENT_H
#define FS_OBJECT_COMPONENT_H


#include <CommonDefinitions.h>

#include <FsContainer/Include/IId.h>


namespace fs
{
	namespace Rendering
	{
		class Object;
		class ObjectComponent;


		enum class ObjectComponentType
		{
			Invalid,
			TransformComponent,
			MeshComponent,
		};


		class ObjectComponentId final : public IId
		{
			friend ObjectComponent;

		private:
			virtual void			assignRawId(const uint32 rawId) noexcept override final;

		public:
			const uint32			getRawId() const noexcept;
		};


		class ObjectComponent abstract
		{
			friend Object;

		private:
			static std::atomic<uint32>				_nextRawId;

		public:
													ObjectComponent(const ObjectComponentType type);
			virtual									~ObjectComponent() { __noop; }

		public:
			FS_INLINE const ObjectComponentType		getType() const noexcept { return _type; }
			FS_INLINE const bool					isTypeOf(const ObjectComponentType type) const noexcept { return _type == type; }
			FS_INLINE const ObjectComponentId&		getId() const noexcept { return _id; }
			FS_INLINE Object*						getOwnerObject() const noexcept { return _ownerObject; }
		
		protected:
			Object*									_ownerObject;
			ObjectComponentType						_type;
			ObjectComponentId						_id;
		};
	}
}

#endif // !FS_OBJECT_COMPONENT_H
