#pragma once


#ifndef MINT_I_RENDERER_CONTEXT_H
#define MINT_I_RENDERER_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>
#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/IDxObject.h>

#include <Assets/Include/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;


        class IRendererContext abstract
        {
        public:
                                        IRendererContext(GraphicDevice& graphicDevice);
                                        IRendererContext(GraphicDevice& graphicDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer);
            virtual                     ~IRendererContext();

        public:
            virtual void                initializeShaders() noexcept abstract;
            virtual void                flush() noexcept abstract;
            virtual void                render() noexcept abstract;

        protected:
            void                        flushTransformBuffer() noexcept;
            void                        prepareTransformBuffer() noexcept;

        public:
            void                        setUseMultipleViewports() noexcept;
            const bool                  isUsingMultipleViewports() const noexcept;

        public:
            void                        setPosition(const Float4& position) noexcept;
            void                        setPositionZ(const float s) noexcept;
            void                        setColor(const Color& color) noexcept;
            void                        setColor(const Vector<Color>& colorArray) noexcept;
            virtual void                setClipRect(const Rect& clipRect) noexcept;

        protected:
            const Float4&               getColorInternal(const uint32 index) const noexcept;
            static const float          packBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept;
            static const float          packBits4_28AsFloat(const uint32 _4bits, const uint32 _28bits) noexcept;

        protected:
            GraphicDevice&                          _graphicDevice;
            LowLevelRenderer<VS_INPUT_SHAPE>* const _lowLevelRenderer;

        private:
            bool                                    _doesOwnLowLevelRenderer;

        protected:
            Float4                      _position;
            Vector<Color>               _colorArray;
            Color                       _defaultColor;
            Rect                        _clipRect;

        protected:
            Vector<SB_Transform>        _sbTransformData;

        private:
            bool                        _useMultipleViewports;
        };
    }
}


#include <MintRenderingBase/Include/IRendererContext.inl>


#endif // !MINT_I_RENDERER_CONTEXT_H
