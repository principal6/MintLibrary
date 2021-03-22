#pragma once


#ifndef FS_RENDERER_CONTEXT_H
#define FS_RENDERER_CONTEXT_H


#include <FsRenderingBase/Include/TriangleRenderer.h>

#include <FsRendering/Include/Object.h>

#include <FsLibrary/Assets/CppHlsl/CppHlslConstantBuffers.h>


namespace fs
{
	namespace RenderingBase
	{
		class GraphicDevice;
	}


	namespace Rendering
	{
		class TransformComponent;
		class MeshComponent;
		class CameraObject;


		class ObjectManager final
		{
		public:
																		ObjectManager(fs::RenderingBase::GraphicDevice* const graphicDevice);
																		~ObjectManager();

		public:
			void														initialize() noexcept;

		public:
			fs::Rendering::Object*										createObject();
			fs::Rendering::CameraObject*								createCameraObject();

		private:
			void														destroyObjects();

		private:
			fs::Rendering::Object*										createObjectInternalXXX(fs::Rendering::Object* const object);
		
		public:
			fs::Rendering::TransformComponent*							createTransformComponent();
			fs::Rendering::MeshComponent*								createMeshComponent();
		
		public:
			void														destroyObjectComponents(Object& object);

		public:
			void														registerMeshComponent(fs::Rendering::MeshComponent* const meshComponent);
			void														deregisterMeshComponent(fs::Rendering::MeshComponent* const meshComponent);

		public:
			void														updateScreenSize(const fs::Float2& screenSize);

		public:
			void														renderMeshComponents();

		public:
			const uint32												getObjectCount() const noexcept;

		private:
			fs::RenderingBase::GraphicDevice* const						_graphicDevice;

		private:
			std::vector<fs::Rendering::Object*>							_objectArray;

		private:
			std::vector<fs::Rendering::MeshComponent*>					_meshComponentArray;

		private:
			RenderingBase::TriangleRenderer<RenderingBase::VS_INPUT>	_triangleRenderer;
			fs::RenderingBase::CB_Transform								_cbTransformData;
			fs::RenderingBase::DxObjectId								_cbTransformId;

		private:
			fs::RenderingBase::DxObjectId								_vertexShaderId;
			fs::RenderingBase::DxObjectId								_pixelShaderId;
		};
	}
}


#endif // !FS_RENDERER_CONTEXT_H

