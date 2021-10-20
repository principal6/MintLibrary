#pragma once


#ifndef MINT_I_RENDERER_CONTEXT_H
#define MINT_I_RENDERER_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float2.h>
#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Int2.h>
#include <MintMath/Include/Rect.h>

#include <MintRenderingBase/Include/IDxObject.h>

#include <Assets/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
    namespace Rendering
    {
        class GraphicDevice;


        class IRendererContext abstract
        {
        public:
                                                    IRendererContext() = default;
                                                    IRendererContext(mint::Rendering::GraphicDevice* const graphicDevice);
            virtual                                 ~IRendererContext() = default;

        public:
            virtual void                            initializeShaders() noexcept abstract;
            virtual const bool                      hasData() const noexcept abstract;
            virtual void                            flush() noexcept abstract;
            virtual void                            render() noexcept abstract;
            virtual void                            renderAndFlush() noexcept abstract;
        
        protected:
            void                                    flushTransformBuffer() noexcept;
            void                                    prepareTransformBuffer() noexcept;

        public:
            void                                    setUseMultipleViewports() noexcept;
            const bool                              getUseMultipleViewports() const noexcept;

        public:
            void                                    setPosition(const mint::Float4& position) noexcept;
            void                                    setPositionZ(const float s) noexcept;
            void                                    setColor(const mint::Rendering::Color& color) noexcept;
            void                                    setColor(const mint::Vector<mint::Rendering::Color>& colorArray) noexcept;
            virtual void                            setClipRect(const mint::Rect& clipRect) noexcept;

        protected:
            const mint::Float4&                     getColorInternal(const uint32 index) const noexcept;
            static const float                      packBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept;
            static const float                      packBits4_28AsFloat(const uint32 _4bits, const uint32 _28bits) noexcept;

        protected:
            mint::Rendering::GraphicDevice*     _graphicDevice;
        
        protected:
            mint::Float4                            _position;
            mint::Vector<Rendering::Color>      _colorArray;
            mint::Rendering::Color              _defaultColor;
            mint::Rect                              _clipRect;

        protected:
            mint::Vector<SB_Transform>              _sbTransformData;

        private:
            bool                                    _useMultipleViewports;
        };
    }
}


#include <MintRenderingBase/Include/IRendererContext.inl>


#endif // !MINT_I_RENDERER_CONTEXT_H
