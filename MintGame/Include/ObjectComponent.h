#pragma once


#ifndef _MINT_GAME_OBJECT_COMPONENT_H_
#define _MINT_GAME_OBJECT_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/ID.h>

#include <MintMath/Include/Float4x4.h>
#include <MintMath/Include/Quaternion.h>


namespace mint
{
	namespace Game
	{
		class Object;
		class ObjectComponent;
	}
}

namespace mint
{
	namespace Game
	{
		enum class ObjectComponentType
		{
			Invalid,
			TransformComponent,
			MeshComponent,
			Mesh2DComponent,
		};


		class ObjectComponentID final : public ID32
		{
			friend ObjectComponent;
		};


		class ObjectComponent abstract
		{
			friend Object;

		private:
			static std::atomic<uint32> _nextRawID;

		public:
			ObjectComponent(const ObjectComponentType type);
			virtual ~ObjectComponent() { __noop; }

		public:
			MINT_INLINE ObjectComponentType GetType() const noexcept { return _type; }
			MINT_INLINE bool IsTypeOf(const ObjectComponentType type) const noexcept { return _type == type; }
			MINT_INLINE const ObjectComponentID& GetID() const noexcept { return _id; }
			MINT_INLINE Object* GetOwnerObject() const noexcept { return _ownerObject; }

		protected:
			Object* _ownerObject;
			ObjectComponentType _type;
			ObjectComponentID _id;
		};
	}
}


#endif // !_MINT_GAME_OBJECT_COMPONENT_H_
