#include <MintApp/Include/SceneObject.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintApp/Include/SceneObjectComponent.h>
#include <MintApp/Include/SceneObjectPool.hpp>


namespace mint
{
	SceneObject::SceneObject(const SceneObjectPool* const sceneObjectPool)
		: _sceneObjectPool{ sceneObjectPool }
	{
		__noop;
	}

	SceneObject::~SceneObject()
	{
		__noop;
	}

	void SceneObject::AttachComponent(SceneObjectComponent* const component)
	{
		if (component != nullptr)
		{
			if (GetComponent(component->GetType()) != nullptr)
			{
				MINT_LOG_ERROR("동일한 Type 의 Component 를 Attach 하는 것은 아직 지원되지 않습니다!");
				return;
			}

			component->_ownerObject = this;

			_componentArray.PushBack(component);
		}
	}

	void SceneObject::DetachComponent(SceneObjectComponent* const component)
	{
		if (component == nullptr)
		{
			return;
		}

		int32 foundComponentIndex = -1;
		const int32 componentCount = static_cast<int32>(_componentArray.Size());
		for (int32 componentIndex = 0; componentIndex < componentCount; ++componentIndex)
		{
			if (_componentArray[componentIndex] == component)
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

	uint32 SceneObject::GetComponentCount() const noexcept
	{
		return static_cast<uint32>(_componentArray.Size());
	}

	SceneObjectComponent* SceneObject::GetComponent(const SceneObjectComponentType type) const noexcept
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

	void SceneObject::SetObjectTransform(const Transform& transform) noexcept
	{
		GetObjectTransformComponent()->_transform = transform;
	}

	Transform& SceneObject::GetObjectTransform() noexcept
	{
		return GetObjectTransformComponent()->_transform;
	}

	const Transform& SceneObject::GetObjectTransform() const noexcept
	{
		return GetObjectTransformComponent()->_transform;
	}

	Float4x4 SceneObject::GetObjectTransformMatrix() const noexcept
	{
		return GetObjectTransformComponent()->_transform.ToMatrix();
	}

	TransformComponent* SceneObject::GetObjectTransformComponent() const noexcept
	{
		return static_cast<TransformComponent*>(_componentArray[0]);
	}
}
