#include <stdafx.h>
#include <FsRendering/Include/ObjectManager.h>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/TriangleRenderer.hpp>

#include <FsRendering/Include/MeshComponent.h>
#include "..\Include\ObjectManager.h"


namespace fs
{
	namespace Rendering
	{
		ObjectManager::ObjectManager(fs::RenderingBase::GraphicDevice* const graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _triangleRenderer{ graphicDevice }
		{
			Object::_objectManager = this;
		}
		
		ObjectManager::~ObjectManager()
		{
			__noop;
		}

		void ObjectManager::initialize() noexcept
		{
			fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
			const fs::Language::CppHlsl& cppHlsl = _graphicDevice->getCppHlslSteamData();
			const fs::Language::CppHlslTypeInfo& vsInputTypeInfo = cppHlsl.getTypeInfo(typeid(fs::RenderingBase::VS_INPUT));
			
			_vertexShaderId = shaderPool.pushVertexShader("Assets/Hlsl/", "VsDefault.hlsl", "main", &vsInputTypeInfo, "Assets/HlslBinary/");
			_pixelShaderId = shaderPool.pushNonVertexShader("Assets/Hlsl/", "PsDefault.hlsl", "main", fs::RenderingBase::DxShaderType::PixelShader, "Assets/HlslBinary/");

			fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
			const fs::Language::CppHlslTypeInfo& cbTransformDataTypeInfo = _graphicDevice->getCppHlslConstantBuffers().getTypeInfo(typeid(_cbTransformData));
			_cbTransformId = resourcePool.pushConstantBuffer(reinterpret_cast<const byte*>(&_cbTransformData), sizeof(_cbTransformData), cbTransformDataTypeInfo.getRegisterIndex());
		}

		Object& ObjectManager::createObject()
		{
			fs::Rendering::Object newObject;
			_objectArray.emplace_back(std::move(newObject));
			_objectArray.back().attachComponent(createTransformComponent()); // 모든 Object는 TransformComponent 를 필수로 가집니다.
			return _objectArray.back();
		}

		fs::Rendering::TransformComponent* ObjectManager::createTransformComponent()
		{
			return FS_NEW(fs::Rendering::TransformComponent);
		}

		fs::Rendering::MeshComponent* ObjectManager::createMeshComponent()
		{
			fs::Rendering::MeshComponent* result = FS_NEW(fs::Rendering::MeshComponent);
			_meshComponentArray.emplace_back(std::move(result));
			return _meshComponentArray.back();
		}

		void ObjectManager::destroyObjectComponents(Object& object)
		{
			const uint32 componentCount = static_cast<uint32>(object._componentArray.size());
			for (uint32 componentIndex = 0; componentIndex < componentCount; componentIndex++)
			{
				ObjectComponent*& component = object._componentArray[componentIndex];
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

		void ObjectManager::registerMeshComponent(fs::Rendering::MeshComponent* const meshComponent)
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

			_meshComponentArray.emplace_back(meshComponent);
		}

		void ObjectManager::deregisterMeshComponent(fs::Rendering::MeshComponent* const meshComponent)
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

		void ObjectManager::renderMeshComponents()
		{
			const fs::Int2& windowSize = _graphicDevice->getWindowSize();
			const fs::Float2& windowSizeF = fs::Float2(windowSize);
			fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
			shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::VertexShader, _vertexShaderId);
			shaderPool.bindShaderIfNot(fs::RenderingBase::DxShaderType::PixelShader, _pixelShaderId);

			fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
			fs::RenderingBase::DxResource& cbView = resourcePool.getResource(_graphicDevice->getCbViewId());
			{
				fs::RenderingBase::CB_View& cbViewData = _graphicDevice->getCbViewData();
				cbViewData._cbPerspectiveProjectionMatrix = fs::Float4x4::projectionMatrixPerspective(fs::Math::toRadian(60.0f), 0.1f, 100.0f, windowSizeF._x / windowSizeF._y);
				cbView.updateBuffer(reinterpret_cast<const byte*>(&cbViewData), 1);
				cbView.bindToShader(fs::RenderingBase::DxShaderType::VertexShader, cbView.getRegisterIndex());
			}
			fs::RenderingBase::DxResource& cbTransform = resourcePool.getResource(_cbTransformId);
			{
				cbTransform.bindToShader(fs::RenderingBase::DxShaderType::VertexShader, cbTransform.getRegisterIndex());
			}

			_triangleRenderer.flush();

			auto& trVertexArray = _triangleRenderer.vertexArray();
			auto& trIndexArray = _triangleRenderer.indexArray();

			for (auto& meshComponentIter : _meshComponentArray)
			{
				const MeshComponent* const meshComponent = meshComponentIter;
				_cbTransformData._cbWorldMatrix = meshComponent->getOwnerObject()->getObjectTransformMatrix() * meshComponent->_srt.toMatrix();
				cbTransform.updateBuffer(reinterpret_cast<const byte*>(&_cbTransformData), 1);

				const uint32 vertexCount = meshComponent->getVertexCount();
				const uint32 indexCount = meshComponent->getIndexCount();
				const fs::RenderingBase::VS_INPUT* const vertices = meshComponent->getVertices();
				const fs::RenderingBase::IndexElementType* const indices = meshComponent->getIndices();
				for (uint32 vertexIter = 0; vertexIter < vertexCount; vertexIter++)
				{
					trVertexArray.emplace_back(vertices[vertexIter]);
				}
				for (uint32 indexIter = 0; indexIter < indexCount; indexIter++)
				{
					trIndexArray.emplace_back(indices[indexIter]);
				}
			}

			_triangleRenderer.render();
		}
	}
}
