#pragma once


#include <MintApp/Include/ObjectPool.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintApp/Include/DeltaTimer.h>
#include <MintApp/Include/Object.h>
#include <MintApp/Include/CameraObject.h>


namespace mint
{
	inline ObjectPool::ObjectPool()
	{
		__noop;
	}

	inline ObjectPool::~ObjectPool()
	{
		DestroyObjects();
	}

	MINT_INLINE SharedPtr<Object> ObjectPool::CreateObject()
	{
		return CreateObjectInternal(MakeShared<Object>(Object(this)));
	}

	MINT_INLINE SharedPtr<CameraObject> ObjectPool::CreateCameraObject()
	{
		return CreateObjectInternal(MakeShared<CameraObject>(CameraObject(this)));
	}

	MINT_INLINE void ObjectPool::DestroyObjects()
	{
		const uint32 objectCount = GetObjectCount();
		for (uint32 objectIndex = 0; objectIndex < objectCount; ++objectIndex)
		{
			if (_objects[objectIndex].IsValid())
			{
				DestroyObjectComponents(*_objects[objectIndex]);
			}
		}
		_objects.Clear();
	}

	MINT_INLINE SharedPtr<Object> ObjectPool::CreateObjectInternal(SharedPtr<Object>&& object)
	{
		_objects.PushBack(object);
		object->AttachComponent(MINT_NEW(TransformComponent)); // 모든 Object는 TransformComponent 를 필수로 가집니다.
		return _objects.Back();
	}

	template<typename ComponentType>
	MINT_INLINE ComponentType* ObjectPool::CreateObjectComponent()
	{
		ComponentType* component = MINT_NEW(ComponentType);
		const ObjectComponentType type = component->GetType();
		if (type == ObjectComponentType::MeshComponent)
		{
			_meshComponents.PushBack(component);
		}
		else if (type == ObjectComponentType::Mesh2DComponent)
		{
			_mesh2DComponents.PushBack(component);
		}
		return component;
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
					DeregisterMeshComponent(component);
				}

				MINT_DELETE(component);
			}
		}
	}

	MINT_INLINE void ObjectPool::RegisterMeshComponent(ObjectComponent* const meshComponent)
	{
		if (meshComponent == nullptr)
		{
			return;
		}

		const uint32 meshComponentCount = static_cast<uint32>(_meshComponents.Size());
		for (uint32 meshComponentIndex = 0; meshComponentIndex < meshComponentCount; ++meshComponentIndex)
		{
			if (_meshComponents[meshComponentIndex]->GetID() == meshComponent->GetID())
			{
				return;
			}
		}

		_meshComponents.PushBack(meshComponent);
	}

	MINT_INLINE void ObjectPool::DeregisterMeshComponent(ObjectComponent* const meshComponent)
	{
		if (meshComponent == nullptr)
		{
			return;
		}

		int32 foundIndex = -1;
		const int32 meshComponentCount = static_cast<int32>(_meshComponents.Size());
		for (int32 meshComponentIndex = 0; meshComponentIndex < meshComponentCount; ++meshComponentIndex)
		{
			if (_meshComponents[meshComponentIndex]->GetID() == meshComponent->GetID())
			{
				foundIndex = meshComponentIndex;
				break;
			}
		}

		if (foundIndex >= 0)
		{
			if (foundIndex < meshComponentCount)
			{
				std::swap(_meshComponents[foundIndex], _meshComponents.Back());
			}
			_meshComponents.PopBack();
		}
	}

	MINT_INLINE void ObjectPool::ComputeDeltaTime() const noexcept
	{
		DeltaTimer::GetInstance().ComputeDeltaTimeSec();
	}

	MINT_INLINE void ObjectPool::UpdateScreenSize(const Float2& screenSize)
	{
		const uint32 objectCount = _objects.Size();
		for (uint32 objectIndex = 0; objectIndex < objectCount; ++objectIndex)
		{
			SharedPtr<Object>& object = _objects[objectIndex];
			if (object->IsTypeOf(ObjectType::CameraObject) == true)
			{
				CameraObject* const cameraObject = static_cast<CameraObject*>(object.Get());
				cameraObject->UpdateScreenSize(screenSize);
			}
		}
	}

	MINT_INLINE const Vector<ObjectComponent*>& ObjectPool::GetMeshComponents() const noexcept
	{
		return _meshComponents;
	}

	MINT_INLINE const Vector<ObjectComponent*>& ObjectPool::GetMesh2DComponents() const noexcept
	{
		return _mesh2DComponents;
	}

	MINT_INLINE uint32 ObjectPool::GetObjectCount() const noexcept
	{
		return _objects.Size();
	}

	MINT_INLINE float ObjectPool::GetDeltaTimeSec() const noexcept
	{
		return DeltaTimer::GetInstance().GetDeltaTimeSec();
	}
}
