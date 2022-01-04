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
                                            MeshRenderer(GraphicDevice& graphicDevice);
                                            ~MeshRenderer();

        public:
            void                            render(const ObjectPool& objectPool) noexcept;

        private:
            void                            initialize() noexcept;

        private:
            GraphicDevice&                  _graphicDevice;

        private:
            LowLevelRenderer<VS_INPUT>      _lowLevelRenderer;
            CB_Transform                    _cbTransformData;
            Vector<SB_Material>             _sbMaterialDatas;

        private:
            DxObjectId                      _vsDefaultId;
            DxObjectId                      _gsNormalId;
            DxObjectId                      _gsTriangleEdgeId;
            DxObjectId                      _psDefaultId;
            DxObjectId                      _psTexCoordAsColorId;
        };
    }
}


#endif // !MINT_MESH_RENDERER_H
