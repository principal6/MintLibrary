#pragma once


#ifndef MINT_MESH_RENDERER_H
#define MINT_MESH_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/CppHlsl/CppHlslConstantBuffers.h>

#include <MintRendering/Include/ObjectPool.h>


namespace mint
{
    namespace RenderingBase
    {
        class GraphicDevice;
    }


    namespace Rendering
    {
        class MeshComponent;


        class MeshRenderer final
        {
        public:
                                                        MeshRenderer(mint::RenderingBase::GraphicDevice* const graphicDevice);
                                                        ~MeshRenderer();
        
        public:
            void                                        initialize() noexcept;
            void                                        render(const mint::Rendering::ObjectPool& objectPool) noexcept;

        private:
            mint::RenderingBase::GraphicDevice* const   _graphicDevice;

        private:
            mint::RenderingBase::LowLevelRenderer<mint::RenderingBase::VS_INPUT>    _lowLevelRenderer;
            mint::RenderingBase::CB_Transform                                       _cbTransformData;
            mint::RenderingBase::DxObjectId                                         _cbTransformId;

        private:
            mint::RenderingBase::DxObjectId             _vsDefaultId;
            mint::RenderingBase::DxObjectId             _gsNormalId;
            mint::RenderingBase::DxObjectId             _gsTriangleEdgeId;
            mint::RenderingBase::DxObjectId             _psDefaultId;
            mint::RenderingBase::DxObjectId             _psTexCoordAsColorId;
        };
    }
}


#endif // !MINT_MESH_RENDERER_H
