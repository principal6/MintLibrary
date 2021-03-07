#include <stdafx.h>
#include <FsRendering/Include/Object.h>

#include <FsRendering/Include/ObjectComponent.h>
#include <FsRendering/Include/ObjectManager.h>
#include <FsRendering/Include/TransformComponent.h>


namespace fs
{
	namespace Rendering
	{
		ObjectManager* Object::_objectManager;
		Object::Object()
		{
			__noop;
		}

		Object::~Object()
		{
			_objectManager->destroyObjectComponents(*this);
		}

		void Object::attachComponent(ObjectComponent* const objectComponent)
		{
			if (objectComponent != nullptr)
			{
				if (getComponent(objectComponent->getType()) != nullptr)
				{
					FS_LOG_ERROR("김장원", "동일한 Type 의 Component 를 Attach 하는 것은 아직 지원되지 않습니다!");
					return;
				}

				objectComponent->_ownerObject = this;

				_componentArray.emplace_back(objectComponent);
			}
		}

		void Object::detachComponent(ObjectComponent* const objectComponent)
		{
			if (objectComponent == nullptr)
			{
				return;
			}

			int32 foundComponentIndex = -1;
			const int32 componentCount = static_cast<int32>(_componentArray.size());
			for (int32 componentIndex = 0; componentIndex < componentCount; componentIndex++)
			{
				if (_componentArray[componentIndex] == objectComponent)
				{
					foundComponentIndex = componentIndex;
					break;
				}
			}

			if (0 <= foundComponentIndex)
			{
				if (foundComponentIndex < componentCount - 1)
				{
					std::swap(_componentArray[foundComponentIndex], _componentArray.back());
				}
				_componentArray.back()->_ownerObject = nullptr;
				_componentArray.pop_back();
			}
		}
		
		const uint32 fs::Rendering::Object::getComponentCount() const noexcept
		{
			return static_cast<uint32>(_componentArray.size());
		}

		ObjectComponent* Object::getComponent(const ObjectComponentType type) const noexcept
		{
			const uint32 componentCount = getComponentCount();
			for (uint32 componentIndex = 0; componentIndex < componentCount; componentIndex++)
			{
				if (_componentArray[componentIndex]->getType() == type)
				{
					return _componentArray[componentIndex];
				}
			}
			return nullptr;
		}

		fs::Rendering::Srt& Object::getObjectTransformSrt() noexcept
		{
			return getObjectTransformComponent()->_srt;
		}

		fs::Float4x4 Object::getObjectTransformMatrix() const noexcept
		{
			return getObjectTransformComponent()->_srt.toMatrix();
		}

		TransformComponent* Object::getObjectTransformComponent() const noexcept
		{
			return static_cast<TransformComponent*>(_componentArray[0]);
		}
	}
}
