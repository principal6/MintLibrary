#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_RENDERER_H_
#define _MINT_APP_SCENE_OBJECT_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRendering/Include/LowLevelRenderer.h>

#include <Assets/Include/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
	class SceneObjectPool;

	namespace Rendering
	{
		class GraphicsDevice;
		struct SB_Material;
	}
}

namespace mint
{
	class SceneObjectRenderer final
	{
	public:
		SceneObjectRenderer(Rendering::GraphicsDevice& graphicsDevice);
		~SceneObjectRenderer();

	public:
		void Render(const SceneObjectPool& sceneObjectPool) noexcept;

	private:
		void Initialize() noexcept;
		void Terminate() noexcept;

	private:
		Rendering::GraphicsDevice& _graphicsDevice;

	private:
		Rendering::LowLevelRenderer<Rendering::VS_INPUT> _lowLevelRenderer;
		Rendering::CB_Transform _cbTransformData;
		Vector<Rendering::SB_Material> _sbMaterialDatas;

	private:
		Rendering::GraphicsObjectID _shaderPipelineDefaultID;
		Rendering::GraphicsObjectID _shaderPipelineDrawNormalsID;
		Rendering::GraphicsObjectID _shaderPipelineDrawEdgesID;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_RENDERER_H_
