#pragma once


#include <MintGame/Include/ObjectPool.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintGame/Include/DeltaTimer.h>
#include <MintGame/Include/MeshComponent.h>
#include <MintGame/Include/CameraObject.h>


namespace mint
{
	namespace Game
	{
		inline ObjectPool::ObjectPool()
			: _deltaTimer{ &DeltaTimer::GetInstance() }
		{
			__noop;
		}

		inline ObjectPool::~ObjectPool()
		{
			DestroyObjects();
		}

		MINT_INLINE Object* ObjectPool::CreateObject()
		{
			return CreateObjectInternalXXX(MINT_NEW(Object, this));
		}

		MINT_INLINE CameraObject* ObjectPool::CreateCameraObject()
		{
			return static_cast<CameraObject*>(CreateObjectInternalXXX(MINT_NEW(CameraObject, this)));
		}

		MINT_INLINE void ObjectPool::DestroyObjects()
		{
			const uint32 objectCount = GetObjectCount();
			for (uint32 objectIndex = 0; objectIndex < objectCount; ++objectIndex)
			{
				if (_objectArray[objectIndex] != nullptr)
				{
					DestroyObjectComponents(*_objectArray[objectIndex]);

					MINT_DELETE(_objectArray[objectIndex]);
				}
			}
			_objectArray.Clear();
		}

		MINT_INLINE Object* ObjectPool::CreateObjectInternalXXX(Object* const object)
		{
			_objectArray.PushBack(object);
			object->AttachComponent(CreateTransformComponent()); // 모든 Object는 TransformComponent 를 필수로 가집니다.
			return object;
		}

		MINT_INLINE TransformComponent* ObjectPool::CreateTransformComponent()
		{
			return MINT_NEW(TransformComponent);
		}

		MINT_INLINE MeshComponent* ObjectPool::CreateMeshComponent()
		{
			MeshComponent* result = MINT_NEW(MeshComponent);
			_meshComponentArray.PushBack(std::move(result));
			return _meshComponentArray.Back();
		}

		MINT_INLINE void ObjectPool::DestroyObjectComponents(Object& object)
		{
			const uint32 componentCount = static_cast<uint32>(object._componentArray.Size());
			for (uint32 componentIndex = 0; componentIndex < componentCount; ++componentIndex)
			{
				ObjectComponent*& component = object._componentArray[componentIndex];
				if (component != nullptr)
				{
					const ObjectComponentType componentType = component->GetType();
					if (componentType == ObjectComponentType::MeshComponent)
					{
						DeregisterMeshComponent(static_cast<MeshComponent*>(component));
					}

					MINT_DELETE(component);
				}
			}
		}

		MINT_INLINE void ObjectPool::RegisterMeshComponent(MeshComponent* const meshComponent)
		{
			if (meshComponent == nullptr)
			{
				return;
			}

			const uint32 meshComponentCount = static_cast<uint32>(_meshComponentArray.Size());
			for (uint32 meshComponentIndex = 0; meshComponentIndex < meshComponentCount; ++meshComponentIndex)
			{
				if (_meshComponentArray[meshComponentIndex]->GetID() == meshComponent->GetID())
				{
					return;
				}
			}

			_meshComponentArray.PushBack(meshComponent);
		}

		MINT_INLINE void ObjectPool::DeregisterMeshComponent(MeshComponent* const meshComponent)
		{
			if (meshComponent == nullptr)
			{
				return;
			}

			int32 foundIndex = -1;
			const int32 meshComponentCount = static_cast<int32>(_meshComponentArray.Size());
			for (int32 meshComponentIndex = 0; meshComponentIndex < meshComponentCount; ++meshComponentIndex)
			{
				if (_meshComponentArray[meshComponentIndex]->GetID() == meshComponent->GetID())
				{
					foundIndex = meshComponentIndex;
					break;
				}
			}

			if (foundIndex >= 0)
			{
				if (foundIndex < meshComponentCount)
				{
					std::swap(_meshComponentArray[foundIndex], _meshComponentArray.Back());
				}
				_meshComponentArray.PopBack();
			}
		}

		MINT_INLINE void ObjectPool::ComputeDeltaTime() const noexcept
		{
			_deltaTimer->ComputeDeltaTimeSec();
		}

		MINT_INLINE void ObjectPool::UpdateScreenSize(const Float2& screenSize)
		{
			const float screenRatio = (screenSize._x / screenSize._y);
			const uint32 objectCount = _objectArray.Size();
			for (uint32 objectIndex = 0; objectIndex < objectCount; ++objectIndex)
			{
				Object*& object = _objectArray[objectIndex];
				if (object->IsTypeOf(ObjectType::CameraObject) == true)
				{
					CameraObject* const cameraObject = static_cast<CameraObject*>(object);
					cameraObject->SetPerspectiveScreenRatio(screenRatio);
				}
			}
		}

		MINT_INLINE const Vector<MeshComponent*>& ObjectPool::GetMeshComponents() const noexcept
		{
			return _meshComponentArray;
		}

		MINT_INLINE uint32 ObjectPool::GetObjectCount() const noexcept
		{
			return _objectArray.Size();
		}

		MINT_INLINE const DeltaTimer* ObjectPool::GetDeltaTimerXXX() const noexcept
		{
			return _deltaTimer;
		}
	}
}
