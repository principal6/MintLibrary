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

#include <MintRenderingBase/Include/IDxObject.h>

#include <Assets/CppHlsl/CppHlslStructuredBuffers.h>


namespace mint
{
    namespace RenderingBase
    {
        class GraphicDevice;


        class IRendererContext abstract
        {
        public:
                                                    IRendererContext() = default;
                                                    IRendererContext(mint::RenderingBase::GraphicDevice* const graphicDevice);
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
            void                                    setColor(const mint::RenderingBase::Color& color) noexcept;
            void                                    setColor(const mint::Vector<mint::RenderingBase::Color>& colorArray) noexcept;
            virtual void                            setViewportIndex(const uint32 viewportIndex) noexcept;

        protected:
            const mint::Float4&                     getColorInternal(const uint32 index) const noexcept;
            static const float                      packBits2_30AsFloat(const uint32 _2bits, const uint32 _30bits) noexcept;

        protected:
            mint::RenderingBase::GraphicDevice*     _graphicDevice;
        
        protected:
            mint::Float4                            _position;
            mint::Vector<RenderingBase::Color>      _colorArray;
            mint::RenderingBase::Color              _defaultColor;
            float                                   _viewportIndex;

        protected:
            mint::Vector<SB_Transform>              _sbTransformData;

        private:
            bool                                    _useMultipleViewports;
        };
    }
}


#include <MintRenderingBase/Include/IRendererContext.inl>


#endif // !MINT_I_RENDERER_CONTEXT_H
