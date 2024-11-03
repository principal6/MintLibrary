#pragma once


#ifndef _MINT_GAME_MESH_RENDERER_H_
#define _MINT_GAME_MESH_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/Include/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
	class MeshComponent;
	class ObjectPool;

	namespace Rendering
	{
		class GraphicsDevice;
		struct SB_Material;
	}
}

namespace mint
{
	class ObjectRenderer final
	{
	public:
		ObjectRenderer(Rendering::GraphicsDevice& graphicsDevice);
		~ObjectRenderer();

	public:
		void Render(const ObjectPool& objectPool) noexcept;

	private:
		void Initialize() noexcept;

	private:
		Rendering::GraphicsDevice& _graphicsDevice;

	private:
		Rendering::LowLevelRenderer<Rendering::VS_INPUT> _lowLevelRenderer;
		Rendering::CB_Transform _cbTransformData;
		Vector<Rendering::SB_Material> _sbMaterialDatas;

	private:
		Rendering::GraphicsObjectID _inputLayoutDefaultID;
		Rendering::GraphicsObjectID _vsDefaultID;
		Rendering::GraphicsObjectID _gsNormalID;
		Rendering::GraphicsObjectID _gsTriangleEdgeID;
		Rendering::GraphicsObjectID _psDefaultID;
		Rendering::GraphicsObjectID _psTexCoordAsColorID;
	};
}


#endif // !_MINT_GAME_MESH_RENDERER_H_
