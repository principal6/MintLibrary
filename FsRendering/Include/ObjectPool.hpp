#include <stdafx.h>
#include <FsRendering/Include/ObjectPool.h>

#include <FsContainer/Include/Vector.hpp>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/LowLevelRenderer.hpp>

#include <FsRendering/Include/MeshComponent.h>
#include <FsRendering/Include/CameraObject.h>
#include <FsRendering/Include/DeltaTimer.h>


namespace fs
{
    namespace Rendering
    {
        inline ObjectPool::ObjectPool()
            : _deltaTimer{ &fs::Rendering::DeltaTimer::getDeltaTimer() }
        {
            __noop;
        }
        
        inline ObjectPool::~ObjectPool()
        {
            destroyObjects();
        }

        FS_INLINE fs::Rendering::Object* ObjectPool::createObject()
        {
            return createObjectInternalXXX(FS_NEW(fs::Rendering::Object, this));
        }

        FS_INLINE fs::Rendering::CameraObject* ObjectPool::createCameraObject()
        {
            return static_cast<fs::Rendering::CameraObject*>(createObjectInternalXXX(FS_NEW(fs::Rendering::CameraObject, this)));
        }

        FS_INLINE void ObjectPool::destroyObjects()
        {
            const uint32 objectCount = getObjectCount();
            for (uint32 objectIndex = 0; objectIndex < objectCount; objectIndex++)
            {
                if (_objectArray[objectIndex] != nullptr)
                {
                    destroyObjectComponents(*_objectArray[objectIndex]);

                    FS_DELETE(_objectArray[objectIndex]);
                }
            }
            _objectArray.clear();
        }

        FS_INLINE fs::Rendering::Object* ObjectPool::createObjectInternalXXX(fs::Rendering::Object* const object)
        {
            _objectArray.push_back(object);
            object->attachComponent(createTransformComponent()); // 모든 Object는 TransformComponent 를 필수로 가집니다.
            return object;
        }

        FS_INLINE fs::Rendering::TransformComponent* ObjectPool::createTransformComponent()
        {
            return FS_NEW(fs::Rendering::TransformComponent);
        }

        FS_INLINE fs::Rendering::MeshComponent* ObjectPool::createMeshComponent()
        {
            fs::Rendering::MeshComponent* result = FS_NEW(fs::Rendering::MeshComponent);
            _meshComponentArray.push_back(std::move(result));
            return _meshComponentArray.back();
        }

        FS_INLINE void ObjectPool::destroyObjectComponents(Object& object)
        {
            const uint32 componentCount = static_cast<uint32>(object._componentArray.size());
            for (uint32 componentIndex = 0; componentIndex < componentCount; componentIndex++)
            {
                IObjectComponent*& component = object._componentArray[componentIndex];
                if (component != nullptr)
                {
                    const ObjectComponentType componentType = component->getType();
                    if (componentType == ObjectComponentType::MeshComponent)
                    {
                        deregisterMeshComponent(static_cast<fs::Rendering::MeshComponent*>(component));
                    }

                    FS_DELETE(component);
                }
            }
        }

        FS_INLINE void ObjectPool::registerMeshComponent(fs::Rendering::MeshComponent* const meshComponent)
        {
            if (meshComponent == nullptr)
            {
                return;
            }

            const uint32 meshComponentCount = static_cast<uint32>(_meshComponentArray.size());
            for (uint32 meshComponentIndex = 0; meshComponentIndex < meshComponentCount; meshComponentIndex++)
            {
                if (_meshComponentArray[meshComponentIndex]->getId() == meshComponent->getId())
                {
                    return;
                }
            }

            _meshComponentArray.push_back(meshComponent);
        }

        FS_INLINE void ObjectPool::deregisterMeshComponent(fs::Rendering::MeshComponent* const meshComponent)
        {
            if (meshComponent == nullptr)
            {
                return;
            }

            int32 foundIndex = -1;
            const int32 meshComponentCount = static_cast<int32>(_meshComponentArray.size());
            for (int32 meshComponentIndex = 0; meshComponentIndex < meshComponentCount; meshComponentIndex++)
            {
                if (_meshComponentArray[meshComponentIndex]->getId() == meshComponent->getId())
                {
                    foundIndex = meshComponentIndex;
                    break;
                }
            }

            if (0 <= foundIndex)
            {
                if (foundIndex < meshComponentCount)
                {
                    std::swap(_meshComponentArray[foundIndex], _meshComponentArray.back());
                }
                _meshComponentArray.pop_back();
            }
        }

        FS_INLINE void ObjectPool::computeDeltaTime() const noexcept
        {
            _deltaTimer->computeDeltaTimeS();
        }

        FS_INLINE void ObjectPool::updateScreenSize(const fs::Float2& screenSize)
        {
            const float screenRatio = (screenSize._x / screenSize._y);
            const uint32 objectCount = _objectArray.size();
            for (uint32 objectIndex = 0; objectIndex < objectCount; ++objectIndex)
            {
                fs::Rendering::Object*& object = _objectArray[objectIndex];
                if (object->isTypeOf(ObjectType::CameraObject) == true)
                {
                    CameraObject* const cameraObject = static_cast<CameraObject*>(object);
                    cameraObject->setPerspectiveScreenRatio(screenRatio);
                }
            }
        }

        FS_INLINE const fs::Vector<fs::Rendering::MeshComponent*>& ObjectPool::getMeshComponents() const noexcept
        {
            return _meshComponentArray;
        }
        
        FS_INLINE const uint32 ObjectPool::getObjectCount() const noexcept
        {
            return static_cast<uint32>(_objectArray.size());
        }

        FS_INLINE const DeltaTimer* ObjectPool::getDeltaTimerXXX() const noexcept
        {
            return _deltaTimer;
        }
    }
}
