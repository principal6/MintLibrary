#pragma once


#ifndef _MINT_GAME_OBJECT_POOL_H_
#define _MINT_GAME_OBJECT_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/SharedPtr.h>


namespace mint
{
	class Object;
	class ObjectComponent;
}

namespace mint
{
	class ObjectPool final
	{
	public:
		ObjectPool();
		~ObjectPool();

	public:
		SharedPtr<Object> CreateObject();

	public:
		template<typename ComponentType>
		ComponentType* CreateObjectComponent();

	public:
		void ComputeDeltaTime() const noexcept;
		void UpdateScreenSize(const Float2& screenSize);

	public:
		const Vector<ObjectComponent*>& GetMeshComponents() const noexcept;
		const Vector<ObjectComponent*>& GetMesh2DComponents() const noexcept;

	public:
		uint32 GetObjectCount() const noexcept;
		float GetDeltaTimeSec() const noexcept;

	private:
		SharedPtr<Object> CreateObjectInternal(SharedPtr<Object>&& object);
		void DestroyObjects();
		void DestroyObjectComponents(Object& object);
		void DeregisterComponent(Vector<ObjectComponent*>& components, ObjectComponent* const component);

	private:
		Vector<SharedPtr<Object>> _objects;

	private:
		Vector<ObjectComponent*> _meshComponents;
		Vector<ObjectComponent*> _mesh2DComponents;
	};
}


#endif // !_MINT_GAME_OBJECT_POOL_H_
