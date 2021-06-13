#pragma once


#ifndef MINT_PLOTTER_H
#define MINT_PLOTTER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRenderingBase/Include/ShapeFontRendererContext.h>


namespace mint
{
    namespace Rendering
    {
        class Plotter
        {
            static constexpr mint::Float2 kDefaultSize = mint::Float2(400, 300);
            static constexpr mint::RenderingBase::Color kAutoColorArray[]{
                mint::RenderingBase::Color(    0.0f,    0.5f,    1.0f),
                mint::RenderingBase::Color(    1.0f,  0.125f, 0.0625f),
                mint::RenderingBase::Color(    1.0f,  0.625f,    0.0f),
                mint::RenderingBase::Color(  0.125f,   0.75f,   0.15f),
            };
            static constexpr uint32 kAutoColorCount = _countof(kAutoColorArray);

        public:
            enum class PlotType
            {
                Circle,
                X,
                Triangle,
            };

        public:
                                    Plotter(mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext);
                                    ~Plotter() = default;

        public:
            void                    scatter(const mint::Vector<float>& xData, const mint::Vector<float>& yData);

        private:
            void                    updateFrameValues();

        public:
            void                    plotType(const PlotType nextPlotType) noexcept { _nextPlotType = nextPlotType; }
            void                    xLabel(const std::wstring& label) noexcept { _xLabel = label; }
            void                    yLabel(const std::wstring& label) noexcept { _yLabel = label; }
            void                    render() noexcept;

        private:
            void                    plotScatter(const PlotType plotType, const float x, const float y, const mint::RenderingBase::Color& color);
            mint::Float2              computeOrigin() const noexcept;
            mint::Float2              computeInFramePlotPosition(const float x, const float y) const noexcept;
            mint::Float2              computePlotPosition(const float x, const float y) const noexcept;
            void                    drawFrame(const mint::Float4& frameCenterPosition) noexcept;
            void                    drawLabels(const mint::Float4& frameCenterPosition) noexcept;

        private:
            mint::RenderingBase::ShapeFontRendererContext* const _shapeFontRendererContext;
        
        private:
            mint::Vector<mint::Vector<float>>           _xDataSets;
            mint::Vector<mint::Vector<float>>           _yDataSets;
            mint::Vector<mint::RenderingBase::Color>    _colorArray;
        
        private:
            mint::Vector<PlotType>    _plotTypeArray;
            PlotType                _nextPlotType;
        
        private:
            mint::Float2              _min;
            mint::Float2              _max;
            mint::Float2              _range;
            mint::Float2              _framePadding;
            mint::Float2              _frameMin;
            mint::Float2              _frameMax;
            mint::Float2              _frameRange;

        private:
            std::wstring            _xLabel;
            std::wstring            _yLabel;
            mint::Float2              _position;
            mint::Float2              _size;
            mint::Float2              _frameOffset;
        };
    }
}


#endif // !MINT_PLOTTER_H
