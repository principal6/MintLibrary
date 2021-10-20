#pragma once


#ifndef MINT_RENDERING_PLOTTER_H
#define MINT_RENDERING_PLOTTER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
    namespace Rendering
    {
        class ShapeFontRendererContext;


        class Plotter
        {
            static constexpr Float2 kDefaultSize = Float2(400, 300);
            static constexpr Color kAutoColorArray[]{
                Color(    0.0f,    0.5f,    1.0f),
                Color(    1.0f,  0.125f, 0.0625f),
                Color(    1.0f,  0.625f,    0.0f),
                Color(  0.125f,   0.75f,   0.15f),
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
                                            Plotter(ShapeFontRendererContext& shapeFontRendererContext);
                                            ~Plotter() = default;

        public:
            void                            scatter(const Vector<float>& xData, const Vector<float>& yData);

        private:
            void                            updateFrameValues();

        public:
            MINT_INLINE void                plotType(const PlotType nextPlotType) noexcept { _nextPlotType = nextPlotType; }
            MINT_INLINE void                xLabel(const std::wstring& label) noexcept { _xLabel = label; }
            MINT_INLINE void                yLabel(const std::wstring& label) noexcept { _yLabel = label; }
            void                            render() noexcept;

        private:
            void                            plotScatter(const PlotType plotType, const float x, const float y, const Color& color);
            Float2                          computeOrigin() const noexcept;
            Float2                          computeInFramePlotPosition(const float x, const float y) const noexcept;
            Float2                          computePlotPosition(const float x, const float y) const noexcept;
            void                            drawFrame(const Float4& frameCenterPosition) noexcept;
            void                            drawLabels(const Float4& frameCenterPosition) noexcept;

        private:
            ShapeFontRendererContext* const _shapeFontRendererContext;
        
        private:
            Vector<Vector<float>>           _xDataSets;
            Vector<Vector<float>>           _yDataSets;
            Vector<Color>                   _colorArray;
        
        private:
            Vector<PlotType>                _plotTypeArray;
            PlotType                        _nextPlotType;
        
        private:
            Float2                          _min;
            Float2                          _max;
            Float2                          _range;
            Float2                          _framePadding;
            Float2                          _frameMin;
            Float2                          _frameMax;
            Float2                          _frameRange;

        private:
            std::wstring                    _xLabel;
            std::wstring                    _yLabel;
            Float2                          _position;
            Float2                          _size;
            Float2                          _frameOffset;
        };
    }
}


#endif // !MINT_RENDERING_PLOTTER_H
