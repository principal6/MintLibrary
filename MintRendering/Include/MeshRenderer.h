#pragma once


#ifndef MINT_MESH_RENDERER_H
#define MINT_MESH_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/CppHlsl/CppHlslConstantBuffers.h>

#include <MintRendering/Include/ObjectPool.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;
        struct SB_Material;
        class MeshComponent;


        class MeshRenderer final
        {
        public:
                                                        MeshRenderer(mint::Rendering::GraphicDevice* const graphicDevice);
                                                        ~MeshRenderer();
        
        public:
            void                                        initialize() noexcept;
            void                                        render(const mint::Rendering::ObjectPool& objectPool) noexcept;

        private:
            mint::Rendering::GraphicDevice* const   _graphicDevice;

        private:
            mint::Rendering::LowLevelRenderer<mint::Rendering::VS_INPUT>    _lowLevelRenderer;
            mint::Rendering::CB_Transform                                       _cbTransformData;
            mint::Vector<mint::Rendering::SB_Material>                          _sbMaterialDatas;

        private:
            mint::Rendering::DxObjectId             _vsDefaultId;
            mint::Rendering::DxObjectId             _gsNormalId;
            mint::Rendering::DxObjectId             _gsTriangleEdgeId;
            mint::Rendering::DxObjectId             _psDefaultId;
            mint::Rendering::DxObjectId             _psTexCoordAsColorId;
        };
    }
}


#endif // !MINT_MESH_RENDERER_H
