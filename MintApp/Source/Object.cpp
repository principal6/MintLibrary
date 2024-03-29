﻿#include <MintApp/Include/Object.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintApp/Include/ObjectComponent.h>
#include <MintApp/Include/ObjectPool.hpp>


namespace mint
{
	Object::Object(const ObjectPool* const objectPool)
		: _objectPool{ objectPool }
	{
		__noop;
	}

	Object::~Object()
	{
		__noop;
	}

	void Object::AttachComponent(ObjectComponent* const objectComponent)
	{
		if (objectComponent != nullptr)
		{
			if (GetComponent(objectComponent->GetType()) != nullptr)
			{
				MINT_LOG_ERROR("동일한 Type 의 Component 를 Attach 하는 것은 아직 지원되지 않습니다!");
				return;
			}

			objectComponent->_ownerObject = this;

			_componentArray.PushBack(objectComponent);
		}
	}

	void Object::DetachComponent(ObjectComponent* const objectComponent)
	{
		if (objectComponent == nullptr)
		{
			return;
		}

		int32 foundComponentIndex = -1;
		const int32 componentCount = static_cast<int32>(_componentArray.Size());
		for (int32 componentIndex = 0; componentIndex < componentCount; ++componentIndex)
		{
			if (_componentArray[componentIndex] == objectComponent)
			{
				foundComponentIndex = componentIndex;
				break;
			}
		}

		if (foundComponentIndex >= 0)
		{
			if (foundComponentIndex < componentCount - 1)
			{
				std::swap(_componentArray[foundComponentIndex], _componentArray.Back());
			}
			_componentArray.Back()->_ownerObject = nullptr;
			_componentArray.PopBack();
		}
	}

	uint32 Object::GetComponentCount() const noexcept
	{
		return static_cast<uint32>(_componentArray.Size());
	}

	ObjectComponent* Object::GetComponent(const ObjectComponentType type) const noexcept
	{
		const uint32 componentCount = GetComponentCount();
		for (uint32 componentIndex = 0; componentIndex < componentCount; ++componentIndex)
		{
			if (_componentArray[componentIndex]->GetType() == type)
			{
				return _componentArray[componentIndex];
			}
		}
		return nullptr;
	}

	void Object::SetObjectTransform(const Transform& transform) noexcept
	{
		GetObjectTransformComponent()->_transform = transform;
	}

	Transform& Object::GetObjectTransform() noexcept
	{
		return GetObjectTransformComponent()->_transform;
	}

	const Transform& Object::GetObjectTransform() const noexcept
	{
		return GetObjectTransformComponent()->_transform;
	}

	Float4x4 Object::GetObjectTransformMatrix() const noexcept
	{
		return GetObjectTransformComponent()->_transform.ToMatrix();
	}

	TransformComponent* Object::GetObjectTransformComponent() const noexcept
	{
		return static_cast<TransformComponent*>(_componentArray[0]);
	}
}
