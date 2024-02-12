#pragma once


#ifndef _MINT_GAME_OBJECT_COMPONENT_H_
#define _MINT_GAME_OBJECT_COMPONENT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/ID.h>

#include <MintMath/Include/Transform.h>


namespace mint
{
	class Object;
	class ObjectComponent;
}

namespace mint
{
	enum class ObjectComponentType
	{
		Invalid,
		TransformComponent,
		MeshComponent,
		Mesh2DComponent,
		Collision2DComponent,
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

	class TransformComponent : public ObjectComponent
	{
	public:
		TransformComponent() : TransformComponent(ObjectComponentType::TransformComponent) { __noop; }
		TransformComponent(const ObjectComponentType derivedType) : ObjectComponent(derivedType) { __noop; }
		virtual ~TransformComponent() { __noop; }

	public:
		Transform _transform;
	};
}


#endif // !_MINT_GAME_OBJECT_COMPONENT_H_
