#pragma once


#ifndef MINT_INSTANT_RENDERER_H
#define MINT_INSTANT_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;
        struct SB_Material;
        struct SRT;
        class Color;


        class InstantRenderer final
        {
        public:
                        InstantRenderer(GraphicDevice* const graphicDevice);
                        ~InstantRenderer();

        public:
            void        initialize() noexcept;

        public:
            void        drawLine(const Float3& a, const Float3& b, const Color& color) noexcept;
            void        drawBox(const SRT& worldSRT, const Float3& extents, const Color& color) noexcept;
            void        drawCone(const SRT& worldSRT, const float radius, const float height, const uint8 detail, const Color& color) noexcept;
            void        drawCylinder(const SRT& worldSRT, const float radius, const float height, const uint8 subdivisionIteration, const Color& color) noexcept;
            void        drawSphere(const Float3& center, const float radius, const uint8 polarDetail, const uint8 azimuthalDetail, const Color& color) noexcept;
            void        drawGeoSphere(const Float3& center, const float radius, const uint8 subdivisionIteration, const Color& color) noexcept;
            void        drawCapsule(const SRT& worldSRT, const float sphereRadius, const float cylinderHeight, const uint8 subdivisionIteration, const Color& color) noexcept;

        private:
            void        pushMeshWithMaterial(MeshData& meshData, const Color& diffuseColor) noexcept;

        public:
            void        render() noexcept;

        private:
            GraphicDevice* const            _graphicDevice;

        private:
            LowLevelRenderer<VS_INPUT>      _lowLevelRendererLine;
            LowLevelRenderer<VS_INPUT>      _lowLevelRendererMesh;

        private:
            CB_Transform                    _cbTransformData;
            Vector<SB_Material>             _sbMaterialDatas;
            DxObjectId                      _vsDefaultId;
            DxObjectId                      _psDefaultId;
            DxObjectId                      _psColorId;
        };
    }
}


#endif // !MINT_INSTANT_RENDERER_H
