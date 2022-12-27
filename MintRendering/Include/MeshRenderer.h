#pragma once


#ifndef _MINT_RENDERING_MESH_RENDERER_H_
#define _MINT_RENDERING_MESH_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/Include/CppHlsl/CppHlslConstantBuffers.h>

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
            GraphicObjectID                      _vsDefaultID;
            GraphicObjectID                      _gsNormalID;
            GraphicObjectID                      _gsTriangleEdgeID;
            GraphicObjectID                      _psDefaultID;
            GraphicObjectID                      _psTexCoordAsColorID;
        };
    }
}


#endif // !_MINT_RENDERING_MESH_RENDERER_H_
