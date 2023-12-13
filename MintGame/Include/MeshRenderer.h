﻿#pragma once


#ifndef _MINT_GAME_MESH_RENDERER_H_
#define _MINT_GAME_MESH_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/Include/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicDevice;
		struct SB_Material;
	}

	namespace Game
	{
		class MeshComponent;
		class ObjectPool;
	}
}
		
namespace mint
{
	namespace Game
	{
		class MeshRenderer final
		{
		public:
			MeshRenderer(Rendering::GraphicDevice& graphicDevice);
			~MeshRenderer();

		public:
			void Render(const ObjectPool& objectPool) noexcept;

		private:
			void Initialize() noexcept;

		private:
			Rendering::GraphicDevice& _graphicDevice;

		private:
			Rendering::LowLevelRenderer<Rendering::VS_INPUT> _lowLevelRenderer;
			Rendering::CB_Transform _cbTransformData;
			Vector<Rendering::SB_Material> _sbMaterialDatas;

		private:
			Rendering::GraphicObjectID _inputLayoutDefaultID;
			Rendering::GraphicObjectID _vsDefaultID;
			Rendering::GraphicObjectID _gsNormalID;
			Rendering::GraphicObjectID _gsTriangleEdgeID;
			Rendering::GraphicObjectID _psDefaultID;
			Rendering::GraphicObjectID _psTexCoordAsColorID;
		};
	}
}


#endif // !_MINT_GAME_MESH_RENDERER_H_
