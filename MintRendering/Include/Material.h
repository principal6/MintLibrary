#pragma once


#ifndef _MINT_RENDERING_BASE_MATERIAL_H_
#define _MINT_RENDERING_BASE_MATERIAL_H_


#include <MintRendering/Include/GraphicsObject.h>

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
			GraphicsObjectID _baseColorTextureID;
			uint32 _baseColorTextureSlot = 0;
			Color _baseColor;
		};

		class Material : public GraphicsObject
		{
			friend MaterialPool;

		public:
			Material(GraphicsDevice& graphicsDevice);
			~Material();

		public:
			MINT_INLINE GraphicsObjectID GetShaderPipelineID() const { return _shaderPipelineID; }
			MINT_INLINE GraphicsObjectID GetBaseColorTextureID() const { return _baseColorTextureID; }
			MINT_INLINE uint32 GetBaseColorTextureSlot() const { return _baseColorTextureSlot; }
			MINT_INLINE const Color& GetBaseColor() const { return _baseColor; }

		private:
			StackStringA<128> _materialName;
			GraphicsObjectID _shaderPipelineID;
			GraphicsObjectID _baseColorTextureID;
			uint32 _baseColorTextureSlot;
			Color _baseColor;
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
			const Material* GetMaterial(const GraphicsObjectID& materialID) const noexcept;

		private:
			GraphicsDevice& _graphicsDevice;
			Vector<OwnPtr<Material>> _materials;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_MATERIAL_H_
