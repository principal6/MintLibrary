#pragma once


#ifndef _MINT_RENDERING_BASE_MATERIAL_H_
#define _MINT_RENDERING_BASE_MATERIAL_H_


#include <MintRenderingBase/Include/GraphicsObject.h>

#include <MintContainer/Include/StackString.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicsDevice;
		class MaterialPool;
	}
}

namespace mint
{
	namespace Rendering
	{
		struct MaterialDesc
		{
			StackStringA<128> _materialName;
			GraphicsObjectID _shaderPipelineID;
			GraphicsObjectID _textureID;
			uint32 _textureSlot;
		};

		class Material : public GraphicsObject
		{
			friend MaterialPool;

		public:
			Material(GraphicsDevice& graphicsDevice);
			~Material();

		private:
			StackStringA<128> _materialName;
			GraphicsObjectID _shaderPipelineID;
			GraphicsObjectID _textureID;
			uint32 _textureSlot;
		};


		class MaterialPool
		{
			friend GraphicsDevice;

		private:
			MaterialPool(GraphicsDevice& graphicsDevice);

		public:
			~MaterialPool();

		public:
			GraphicsObjectID CreateMaterial(const MaterialDesc& materialDesc) noexcept;
			void DestroyMaterial(const GraphicsObjectID& materialID) noexcept;
			void BindMaterial(const GraphicsObjectID& materialID) noexcept;

		private:
			GraphicsDevice& _graphicsDevice;
			Vector<OwnPtr<Material>> _materials;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_MATERIAL_H_
