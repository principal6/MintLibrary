#include <stdafx.h>
#include <FsRendering/Include/Object.h>

#include <FsContainer/Include/Vector.hpp>

#include <FsRendering/Include/IObjectComponent.h>
#include <FsRendering/Include/ObjectPool.hpp>
#include <FsRendering/Include/TransformComponent.h>


namespace fs
{
    namespace Rendering
    {
        Object::Object(const ObjectPool* const objectPool)
            : Object(objectPool, ObjectType::Object)
        {
            __noop;
        }

        Object::Object(const ObjectPool* const objectPool, const ObjectType objectType)
            : _objectPool{ objectPool }
            , _objectType{ objectType }
        {
            __noop;
        }

        Object::~Object()
        {
            __noop;
        }

        void Object::attachComponent(IObjectComponent* const objectComponent)
        {
            if (objectComponent != nullptr)
            {
                if (getComponent(objectComponent->getType()) != nullptr)
                {
                    FS_LOG_ERROR("김장원", "동일한 Type 의 Component 를 Attach 하는 것은 아직 지원되지 않습니다!");
                    return;
                }

                objectComponent->_ownerObject = this;

                _componentArray.push_back(objectComponent);
            }
        }

        void Object::detachComponent(IObjectComponent* const objectComponent)
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

        IObjectComponent* Object::getComponent(const ObjectComponentType type) const noexcept
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

        const fs::Rendering::Srt& Object::getObjectTransformSrt() const noexcept
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

        const float Object::getDeltaTimeS() const noexcept
        {
            const DeltaTimer& deltaTimer = *_objectPool->getDeltaTimerXXX();
            return deltaTimer.getDeltaTimeS();
        }
    }
}
