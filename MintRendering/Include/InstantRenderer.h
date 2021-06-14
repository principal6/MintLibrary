#pragma once


#ifndef MINT_INSTANT_RENDERER_H
#define MINT_INSTANT_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <Assets/CppHlsl/CppHlslConstantBuffers.h>


namespace mint
{
    namespace RenderingBase
    {
        class GraphicDevice;
    }


    namespace Rendering
    {
        class InstantRenderer final
        {
        public:
                                                        InstantRenderer(mint::RenderingBase::GraphicDevice* const graphicDevice);
                                                        ~InstantRenderer();

        public:
            void                                        initialize() noexcept;

        public:
            void                                        drawLine(const mint::Float3& a, const mint::Float3& b) noexcept;
            void                                        drawSphere(const mint::Float3& center, const float radius, const uint8 subdivisionIteration) noexcept;

        public:
            void                                        render() noexcept;

        private:
            mint::RenderingBase::GraphicDevice* const   _graphicDevice;

        private:
            mint::RenderingBase::LowLevelRenderer<mint::RenderingBase::VS_INPUT>    _lowLevelRendererLine;
            mint::RenderingBase::LowLevelRenderer<mint::RenderingBase::VS_INPUT>    _lowLevelRendererMesh;

        private:
            mint::RenderingBase::CB_Transform           _cbTransformData;
            mint::RenderingBase::DxObjectId             _cbTransformId;
            mint::RenderingBase::DxObjectId             _vsDefaultId;
            mint::RenderingBase::DxObjectId             _psDefaultId;
        };
    }
}


#endif // !MINT_INSTANT_RENDERER_H
