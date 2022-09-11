#include <stdafx.h>
#include <MintRendering/Include/Object.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRendering/Include/IObjectComponent.h>
#include <MintRendering/Include/ObjectPool.hpp>
#include <MintRendering/Include/TransformComponent.h>


namespace mint
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
                    MINT_LOG_ERROR("동일한 Type 의 Component 를 Attach 하는 것은 아직 지원되지 않습니다!");
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
                    std::swap(_componentArray[foundComponentIndex], _componentArray.back());
                }
                _componentArray.back()->_ownerObject = nullptr;
                _componentArray.pop_back();
            }
        }

        uint32 Object::getComponentCount() const noexcept
        {
            return static_cast<uint32>(_componentArray.size());
        }

        IObjectComponent* Object::getComponent(const ObjectComponentType type) const noexcept
        {
            const uint32 componentCount = getComponentCount();
            for (uint32 componentIndex = 0; componentIndex < componentCount; ++componentIndex)
            {
                if (_componentArray[componentIndex]->getType() == type)
                {
                    return _componentArray[componentIndex];
                }
            }
            return nullptr;
        }

        void Object::setObjectTransform(const Transform& transform) noexcept
        {
            getObjectTransformComponent()->_transform = transform;
        }

        Transform& Object::getObjectTransform() noexcept
        {
            return getObjectTransformComponent()->_transform;
        }

        const Transform& Object::getObjectTransform() const noexcept
        {
            return getObjectTransformComponent()->_transform;
        }

        Float4x4 Object::getObjectTransformMatrix() const noexcept
        {
            return getObjectTransformComponent()->_transform.toMatrix();
        }

        TransformComponent* Object::getObjectTransformComponent() const noexcept
        {
            return static_cast<TransformComponent*>(_componentArray[0]);
        }

        float Object::getDeltaTimeS() const noexcept
        {
            const DeltaTimer& deltaTimer = *_objectPool->getDeltaTimerXXX();
            return deltaTimer.getDeltaTimeS();
        }
    }
}
