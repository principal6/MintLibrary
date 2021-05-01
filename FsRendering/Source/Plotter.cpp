#include <stdafx.h>
#include <FsRendering/Include/Plotter.h>

#include <FsContainer/Include/Vector.hpp>

#include <FsContainer/Include/StringUtil.hpp>


namespace fs
{
    namespace Rendering
    {
        Plotter::Plotter(fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext)
            : _shapeFontRendererContext{ &shapeFontRendererContext }
            , _nextPlotType{ PlotType::Circle }
            , _size{ kDefaultSize }
            , _frameOffset{ 100.0f, 100.0f }
        {
            _min._x = +fs::Math::kFloatMax;
            _min._y = +fs::Math::kFloatMax;

            _max._x = -fs::Math::kFloatMax;
            _max._y = -fs::Math::kFloatMax;
        }

        void Plotter::scatter(const fs::Vector<float>& xData, const fs::Vector<float>& yData)
        {
            _xDataSets.push_back(xData);
            _yDataSets.push_back(yData);

            const uint32 dataCount = xData.size();
            for (uint32 dataIndex = 0; dataIndex < dataCount; ++dataIndex)
            {
                _min._x = fs::min(_min._x, xData[dataIndex]);
                _min._y = fs::min(_min._y, yData[dataIndex]);

                _max._x = fs::max(_max._x, xData[dataIndex]);
                _max._y = fs::max(_max._y, yData[dataIndex]);
            }

            _range = _max - _min;
            
            updateFrameValues();

            _colorArray.push_back(fs::RenderingBase::Color::kTransparent);
            _plotTypeArray.push_back(_nextPlotType);
        }

        void Plotter::updateFrameValues()
        {
            _framePadding = _range / 10.0f;
            _framePadding._x = std::ceilf(_framePadding._x);
            _framePadding._y = std::ceilf(_framePadding._y);

            _frameMin = _min - _framePadding;
            _frameMin._x = std::floorf(_frameMin._x);
            _frameMin._y = std::floorf(_frameMin._y);

            _frameMax = _max + _framePadding;
            _frameMax._x = std::ceilf(_frameMax._x);
            _frameMax._y = std::ceilf(_frameMax._y);

            _frameRange = _frameMax - _frameMin;
        }

        void Plotter::render() noexcept
        {
            const fs::Float2 centerPosition = _position +_size * 0.5f;
            fs::Float4 frameCenterPosition = fs::Float4(centerPosition._x + _frameOffset._x, centerPosition._y + _frameOffset._y, 0.0f, 1.0f);

            drawFrame(frameCenterPosition);
            drawLabels(frameCenterPosition);

            uint32 autoColorIndex = 0;
            const uint32 setCount = _xDataSets.size();
            for (uint32 setIndex = 0; setIndex < setCount; ++setIndex)
            {
                const bool useAutoColor = (_colorArray[setIndex] == fs::RenderingBase::Color::kTransparent);
                const PlotType plotType = _plotTypeArray[setIndex];
                const fs::RenderingBase::Color& color = (useAutoColor == true) ? kAutoColorArray[autoColorIndex] : _colorArray[setIndex];

                const uint32 dataCount = _xDataSets[setIndex].size();
                for (uint32 dataIndex = 0; dataIndex < dataCount; ++dataIndex)
                {
                    const float xDatum = _xDataSets[setIndex][dataIndex];
                    const float yDatum = _yDataSets[setIndex][dataIndex];

                    plotScatter(plotType, xDatum, yDatum, color);
                }

                if (useAutoColor == true)
                {
                    ++autoColorIndex;
                    if (kAutoColorCount <= autoColorIndex)
                    {
                        autoColorIndex = 0;
                    }
                }
            }

            _shapeFontRendererContext->renderAndFlush();
        }
        
        void Plotter::plotScatter(const PlotType plotType, const float x, const float y, const fs::RenderingBase::Color& color)
        {
            const fs::Float2 plotPosition2 = computePlotPosition(x, y);
            
            _shapeFontRendererContext->setColor(color);
            _shapeFontRendererContext->setPosition(fs::Float4(plotPosition2._x, plotPosition2._y, 0.0f, 1.0f));

            switch (plotType)
            {
            case fs::Rendering::Plotter::PlotType::Circle:
            {
                const float kRadius = 4.0f;
                _shapeFontRendererContext->drawCircle(kRadius);
                break;
            }   
            case fs::Rendering::Plotter::PlotType::X:
            {
                const float kLength = 4.0f;
                const float kHalfLength = kLength * 0.5f;
                const float kThickness = 2.0f;
                _shapeFontRendererContext->drawLine(plotPosition2 - fs::Float2(kHalfLength), plotPosition2 + fs::float2(kLength), kThickness);
                _shapeFontRendererContext->drawLine(plotPosition2 - fs::Float2(kHalfLength, -kLength), plotPosition2 + fs::float2(kLength, -kHalfLength), kThickness);
                break;
            }
            case fs::Rendering::Plotter::PlotType::Triangle:
            {
                static const float kSqrt3 = sqrt(3.0f);
                const float kHeight = 8.0f;
                const float kHalfHeight = kHeight * 0.5f;
                const float kHalfWidth = kHeight / kSqrt3;
                _shapeFontRendererContext->drawSolidTriangle(
                    plotPosition2 + fs::Float2(-kHalfWidth, +kHalfHeight), 
                    plotPosition2 + fs::float2(0.0f, -kHalfHeight),
                    plotPosition2 + fs::Float2(+kHalfWidth, +kHalfHeight));
                break;
            }
            default:
                break;
            }
        }

        fs::Float2 Plotter::computeOrigin() const noexcept
        {
            return _frameOffset + fs::Float2(0.0f, _size._y);
        }

        fs::Float2 Plotter::computeInFramePlotPosition(const float x, const float y) const noexcept
        {
            const float xRatio = (x - _frameMin._x) / _frameRange._x;
            const float yRatio = (y - _frameMin._y) / _frameRange._y;
            return fs::Float2(xRatio * _size._x, yRatio * _size._y);
        }

        fs::Float2 Plotter::computePlotPosition(const float x, const float y) const noexcept
        {
            const fs::Float2 inFramePlotPosition = computeInFramePlotPosition(x, y);
            const fs::Float2 origin = computeOrigin();
            return fs::Float2(origin._x + inFramePlotPosition._x, origin._y - inFramePlotPosition._y);
        }

        void Plotter::drawFrame(const fs::Float4& frameCenterPosition) noexcept
        {
            _shapeFontRendererContext->setBorderColor(fs::RenderingBase::Color::kBlack);
            _shapeFontRendererContext->setColor(fs::RenderingBase::Color::kWhite);
            _shapeFontRendererContext->setPosition(frameCenterPosition);
            _shapeFontRendererContext->drawRectangle(_size, 1.0f, 0.0f);
        }

        void Plotter::drawLabels(const fs::Float4& frameCenterPosition) noexcept
        {
            const float paddingY = 10.0f;
            fs::Float4 labelPosition = fs::Float4(frameCenterPosition._x, frameCenterPosition._y + _size._y * 0.5f + paddingY, 0.0f, 1.0f);
            fs::RenderingBase::FontRenderingOption labelRenderingOption;
            labelRenderingOption._directionHorz = fs::RenderingBase::TextRenderDirectionHorz::Centered;
            _shapeFontRendererContext->setTextColor(fs::RenderingBase::Color::kBlack);
            _shapeFontRendererContext->drawDynamicText(_xLabel.c_str(), static_cast<uint32>(_xLabel.length()), labelPosition, labelRenderingOption);

            const float paddingX = -2.0f;
            const float textWidth = _shapeFontRendererContext->calculateTextWidth(_yLabel.c_str(), static_cast<uint32>(_yLabel.length()));
            labelPosition = fs::Float4(frameCenterPosition._x - _size._x * 0.5f - paddingX, frameCenterPosition._y + textWidth * 0.5f, 0.0f, 1.0f);
            labelRenderingOption._transformMatrix = fs::Float4x4::rotationMatrixZ(-fs::Math::kPiOverTwo);
            _shapeFontRendererContext->drawDynamicText(_yLabel.c_str(), static_cast<uint32>(_yLabel.length()), labelPosition, labelRenderingOption);
        }
    }
}
