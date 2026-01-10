#pragma once


#ifndef _MINT_APP_SCENE_OBJECT_RENDERER_H_
#define _MINT_APP_SCENE_OBJECT_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRendering/Include/LowLevelRenderer.h>

#include <Assets/Include/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
	class SceneObjectRegistry;

	namespace Rendering
	{
		class GraphicsDevice;
		struct CB_Material;
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
		void Render(const SceneObjectRegistry& sceneObjectRegistry) noexcept;

	private:
		void Initialize() noexcept;
		void Terminate() noexcept;

	private:
		Rendering::GraphicsDevice& _graphicsDevice;

	private:
		Rendering::LowLevelRenderer<Rendering::VS_INPUT> _lowLevelRenderer;
		Rendering::CB_Transform _cbTransformData;
		Vector<Rendering::CB_Material> _cbMaterialDatas;

	private:
		Rendering::GraphicsObjectID _shaderPipelineDefaultID;
		Rendering::GraphicsObjectID _shaderPipelineDrawNormalsID;
		Rendering::GraphicsObjectID _shaderPipelineDrawEdgesID;
	};
}


#endif // !_MINT_APP_SCENE_OBJECT_RENDERER_H_
