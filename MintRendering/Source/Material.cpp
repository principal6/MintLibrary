#include <MintRendering/Include/Material.h>

#include <MintContainer/Include/OwnPtr.h>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintContainer/Include/StackString.hpp>


namespace mint
{
	namespace Rendering
	{
		Material::Material(GraphicsDevice& graphicsDevice)
			: GraphicsObject(graphicsDevice, GraphicsObjectType::Material)
			, _baseColorTextureSlot{ 0 }
		{
			__noop;
		}

		Material::~Material()
		{
			__noop;
		}


		MaterialPool::MaterialPool(GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
		{
			__noop;
		}

		MaterialPool::~MaterialPool()
		{
			_materials.Clear();
		}

		GraphicsObjectID MaterialPool::CreateMaterial(const MaterialDesc& materialDesc) noexcept
		{
			for (const OwnPtr<Material>& material : _materials)
			{
				if (material->_materialName == materialDesc._materialName)
				{
					MINT_ASSERT(false, "Material with the same name already exists!");
					return material->GetID();
				}
			}

			OwnPtr<Material> newMaterial = MINT_NEW(Material, _graphicsDevice);
			newMaterial->_materialName = materialDesc._materialName;
			newMaterial->_shaderPipelineID = materialDesc._shaderPipelineID;
			newMaterial->_baseColorTextureID = materialDesc._baseColorTextureID;
			newMaterial->_baseColorTextureSlot = materialDesc._baseColorTextureSlot;
			newMaterial->_baseColor = materialDesc._baseColor;
			newMaterial->AssignIDXXX();
			const GraphicsObjectID graphicsObjectID = newMaterial->GetID();
			_materials.PushBack(std::move(newMaterial));
			QuickSort(_materials, GraphicsObject::AscendingComparator());
			return graphicsObjectID;
		}

		void MaterialPool::DestroyMaterial(const GraphicsObjectID& materialID) noexcept
		{
			const uint32 index = BinarySearch(_materials, materialID, GraphicsObject::Evaluator());
			_materials[index].Release();
			_materials.Erase(index);
		}

		void MaterialPool::BindMaterial(const GraphicsObjectID& materialID) noexcept
		{
			const uint32 index = BinarySearch(_materials, materialID, GraphicsObject::Evaluator());
			MINT_ASSERT(IsValidIndex(index) == true, "Material not found!");

			const Material& material = *_materials[index];
			ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
			shaderPipelinePool.GetShaderPipeline(material._shaderPipelineID).BindShaderPipeline();

			if (material._baseColorTextureID.IsValid() == true)
			{
				GraphicsResourcePool& resourcePool = _graphicsDevice.GetResourcePool();
				resourcePool.GetResource(material._baseColorTextureID).BindToShader(GraphicsShaderType::PixelShader, material._baseColorTextureSlot);
			}
		}
	}
}
