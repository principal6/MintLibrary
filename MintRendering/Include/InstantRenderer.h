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
        struct Color;


        class InstantRenderer final
        {
        public:
                                                        InstantRenderer(mint::Rendering::GraphicDevice* const graphicDevice);
                                                        ~InstantRenderer();

        public:
            void                                        initialize() noexcept;

        public:
            void                                        drawLine(const mint::Float3& a, const mint::Float3& b, const mint::Rendering::Color& color) noexcept;
            void                                        drawSphere(const mint::Float3& center, const float radius, const uint8 subdivisionIteration, const mint::Rendering::Color& color) noexcept;

        public:
            void                                        render() noexcept;

        private:
            mint::Rendering::GraphicDevice* const   _graphicDevice;

        private:
            mint::Rendering::LowLevelRenderer<mint::Rendering::VS_INPUT>    _lowLevelRendererLine;
            mint::Rendering::LowLevelRenderer<mint::Rendering::VS_INPUT>    _lowLevelRendererMesh;

        private:
            mint::Rendering::CB_Transform               _cbTransformData;
            mint::Vector<mint::Rendering::SB_Material>  _sbMaterialDatas;
            mint::Rendering::DxObjectId                 _vsDefaultId;
            mint::Rendering::DxObjectId                 _psDefaultId;
            mint::Rendering::DxObjectId                 _psColorId;
        };
    }
}


#endif // !MINT_INSTANT_RENDERER_H
