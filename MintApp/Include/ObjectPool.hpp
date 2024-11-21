#pragma once


#include <MintApp/Include/ObjectPool.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintApp/Include/DeltaTimer.h>
#include <MintApp/Include/Object.h>


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
					DeregisterComponent(_meshComponents, component);
				}
				else if (componentType == ObjectComponentType::Mesh2DComponent)
				{
					DeregisterComponent(_mesh2DComponents, component);
				}

				MINT_DELETE(component);
			}
		}

		object._componentArray.Clear();
	}

	MINT_INLINE void ObjectPool::DeregisterComponent(Vector<ObjectComponent*>& components, ObjectComponent* const component)
	{
		if (component == nullptr)
		{
			return;
		}

		int32 foundIndex = -1;
		const int32 componentCount = static_cast<int32>(components.Size());
		for (int32 i = 0; i < componentCount; ++i)
		{
			if (components[i]->GetID() == component->GetID())
			{
				foundIndex = i;
				break;
			}
		}

		if (foundIndex >= 0)
		{
			if (foundIndex < componentCount)
			{
				std::swap(components[foundIndex], components.Back());
			}
			components.PopBack();
		}
	}

	MINT_INLINE void ObjectPool::UpdateScreenSize(const Float2& screenSize)
	{
		const uint32 objectCount = _objects.Size();
		for (uint32 objectIndex = 0; objectIndex < objectCount; ++objectIndex)
		{
			SharedPtr<Object>& object = _objects[objectIndex];
			CameraComponent* const cameraComponent = static_cast<CameraComponent*>(object->GetComponent(ObjectComponentType::CameraComponent));
			if (cameraComponent != nullptr)
			{
				cameraComponent->UpdateScreenSize(screenSize);
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
}
