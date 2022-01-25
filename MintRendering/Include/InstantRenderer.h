#pragma once


#ifndef MINT_INSTANT_RENDERER_H
#define MINT_INSTANT_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
    struct Transform;


    namespace Rendering
    {
        class GraphicDevice;
        struct SB_Material;
        class Color;


        class InstantRenderer final
        {
        public:
                        InstantRenderer(GraphicDevice& graphicDevice);
                        ~InstantRenderer();

        public:
            void        testDraw(const Float3& worldOffset) noexcept;

        public:
            void        drawLine(const Float3& a, const Float3& b, const Color& color) noexcept;
            void        drawBox(const Transform& worldTransform, const Float3& extents, const Color& color) noexcept;
            void        drawCone(const Transform& worldTransform, const float radius, const float height, const uint8 detail, const Color& color) noexcept;
            void        drawCylinder(const Transform& worldTransform, const float radius, const float height, const uint8 subdivisionIteration, const Color& color) noexcept;
            void        drawSphere(const Float3& center, const float radius, const uint8 polarDetail, const uint8 azimuthalDetail, const Color& color) noexcept;
            void        drawGeoSphere(const Float3& center, const float radius, const uint8 subdivisionIteration, const Color& color) noexcept;
            void        drawCapsule(const Transform& worldTransform, const float sphereRadius, const float cylinderHeight, const uint8 subdivisionIteration, const Color& color) noexcept;

        private:
            void        initialize() noexcept;
            void        pushMeshWithMaterial(MeshData& meshData, const Color& diffuseColor) noexcept;

        public:
            void        render() noexcept;

        private:
            GraphicDevice&                  _graphicDevice;

        private:
            LowLevelRenderer<VS_INPUT>      _lowLevelRendererLine;
            LowLevelRenderer<VS_INPUT>      _lowLevelRendererMesh;

        private:
            CB_Transform                    _cbTransformData;
            Vector<SB_Material>             _sbMaterialDatas;
            DxObjectID                      _vsDefaultID;
            DxObjectID                      _psDefaultID;
            DxObjectID                      _psColorID;
        };
    }
}


#endif // !MINT_INSTANT_RENDERER_H
