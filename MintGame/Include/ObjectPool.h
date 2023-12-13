#pragma once


#ifndef _MINT_GAME_OBJECT_POOL_H_
#define _MINT_GAME_OBJECT_POOL_H_


#include <MintGame/Include/Object.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
	namespace Game
	{
		class GraphicDevice;
		class TransformComponent;
		class MeshComponent;
		class CameraObject;
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
			Object* CreateObject();
			CameraObject* CreateCameraObject();

		private:
			void DestroyObjects();

		private:
			Object* CreateObjectInternalXXX(Object* const object);

		public:
			TransformComponent* CreateTransformComponent();
			MeshComponent* CreateMeshComponent();

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

		public:
			uint32 GetObjectCount() const noexcept;
			const DeltaTimer* GetDeltaTimerXXX() const noexcept;

		private:
			const DeltaTimer* const _deltaTimer;

		private:
			Vector<Object*> _objectArray;

		private:
			Vector<MeshComponent*> _meshComponentArray;
		};
	}
}


#endif // !_MINT_GAME_OBJECT_POOL_H_
