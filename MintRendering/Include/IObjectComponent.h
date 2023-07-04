﻿#pragma once


#ifndef _MINT_RENDERING_OBJECT_COMPONENT_H_
#define _MINT_RENDERING_OBJECT_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/ID.h>

#include <MintMath/Include/Float4x4.h>
#include <MintMath/Include/Quaternion.h>


namespace mint
{
	namespace Rendering
	{
		class Object;
		class IObjectComponent;


		enum class ObjectComponentType
		{
			Invalid,
			TransformComponent,
			MeshComponent,
		};


		class ObjectComponentID final : public ID32
		{
			friend IObjectComponent;
		};


		class IObjectComponent abstract
		{
			friend Object;

		private:
			static std::atomic<uint32> _nextRawID;

		public:
			IObjectComponent(const ObjectComponentType type);
			virtual ~IObjectComponent() { __noop; }

		public:
			MINT_INLINE ObjectComponentType GetType() const noexcept { return _type; }
			MINT_INLINE bool IsTypeOf(const ObjectComponentType type) const noexcept { return _type == type; }
			MINT_INLINE const ObjectComponentID& GetID() const noexcept { return _id; }
			MINT_INLINE Object* getOwnerObject() const noexcept { return _ownerObject; }

		protected:
			Object* _ownerObject;
			ObjectComponentType _type;
			ObjectComponentID _id;
		};
	}
}


#endif // !_MINT_RENDERING_OBJECT_COMPONENT_H_
