#pragma once


#ifndef FS_PLOTTER_H
#define FS_PLOTTER_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>

#include <FsRenderingBase/Include/ShapeFontRendererContext.h>


namespace fs
{
    namespace Rendering
    {
        class Plotter
        {
            static constexpr fs::Float2 kDefaultSize = fs::Float2(400, 300);
            static constexpr fs::RenderingBase::Color kAutoColorArray[]{
                fs::RenderingBase::Color(    0.0f,    0.5f,    1.0f),
                fs::RenderingBase::Color(    1.0f,  0.125f, 0.0625f),
                fs::RenderingBase::Color(    1.0f,  0.625f,    0.0f),
                fs::RenderingBase::Color(  0.125f,   0.75f,   0.15f),
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
                                    Plotter(fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext);
                                    ~Plotter() = default;

        public:
            void                    scatter(const fs::Vector<float>& xData, const fs::Vector<float>& yData);

        private:
            void                    updateFrameValues();

        public:
            void                    plotType(const PlotType nextPlotType) noexcept { _nextPlotType = nextPlotType; }
            void                    xLabel(const std::wstring& label) noexcept { _xLabel = label; }
            void                    yLabel(const std::wstring& label) noexcept { _yLabel = label; }
            void                    render() noexcept;

        private:
            void                    plotScatter(const PlotType plotType, const float x, const float y, const fs::RenderingBase::Color& color);
            fs::Float2              computeOrigin() const noexcept;
            fs::Float2              computeInFramePlotPosition(const float x, const float y) const noexcept;
            fs::Float2              computePlotPosition(const float x, const float y) const noexcept;
            void                    drawFrame(const fs::Float4& frameCenterPosition) noexcept;
            void                    drawLabels(const fs::Float4& frameCenterPosition) noexcept;

        private:
            fs::RenderingBase::ShapeFontRendererContext* const _shapeFontRendererContext;
        
        private:
            fs::Vector<fs::Vector<float>>           _xDataSets;
            fs::Vector<fs::Vector<float>>           _yDataSets;
            fs::Vector<fs::RenderingBase::Color>    _colorArray;
        
        private:
            fs::Vector<PlotType>    _plotTypeArray;
            PlotType                _nextPlotType;
        
        private:
            fs::Float2              _min;
            fs::Float2              _max;
            fs::Float2              _range;
            fs::Float2              _framePadding;
            fs::Float2              _frameMin;
            fs::Float2              _frameMax;
            fs::Float2              _frameRange;

        private:
            std::wstring            _xLabel;
            std::wstring            _yLabel;
            fs::Float2              _position;
            fs::Float2              _size;
            fs::Float2              _frameOffset;
        };
    }
}


#endif // !FS_PLOTTER_H
