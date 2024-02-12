﻿#pragma once


#ifndef _MINT_GAME_OBJECT_POOL_H_
#define _MINT_GAME_OBJECT_POOL_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintGame/Include/Object.h>


namespace mint
{
	namespace Game
	{
		class CameraObject;
		class TransformComponent;
		class MeshComponent;
		class Mesh2DComponent;
		class Collision2DComponent;
		class DeltaTimer;
	}
}

namespace mint
{
	namespace Game
	{
		class ObjectPool final
		{
		public:
			ObjectPool();
			~ObjectPool();

		public:
			SharedPtr<Object> CreateObject();
			SharedPtr<CameraObject> CreateCameraObject();

		private:
			void DestroyObjects();

		private:
			SharedPtr<Object> CreateObjectInternal(SharedPtr<Object>&& object);

		public:
			template<typename ComponentType>
			ComponentType* CreateObjectComponent();

		public:
			void DestroyObjectComponents(Object& object);

		public:
			void RegisterMeshComponent(MeshComponent* const meshComponent);
			void DeregisterMeshComponent(MeshComponent* const meshComponent);

		public:
			void ComputeDeltaTime() const noexcept;
			void UpdateScreenSize(const Float2& screenSize);

		public:
			const Vector<MeshComponent*>& GetMeshComponents() const noexcept;
			const Vector<Mesh2DComponent*>& GetMesh2DComponents() const noexcept;

		public:
			uint32 GetObjectCount() const noexcept;
			float GetDeltaTimeSec() const noexcept;

		private:
			Vector<SharedPtr<Object>> _objects;

		private:
			Vector<MeshComponent*> _meshComponents;
			Vector<Mesh2DComponent*> _mesh2DComponents;
		};
	}
}


#endif // !_MINT_GAME_OBJECT_POOL_H_
