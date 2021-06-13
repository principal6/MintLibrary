#include <stdafx.h>
#include <MintRendering/Include/Plotter.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
    namespace Rendering
    {
        Plotter::Plotter(mint::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext)
            : _shapeFontRendererContext{ &shapeFontRendererContext }
            , _nextPlotType{ PlotType::Circle }
            , _size{ kDefaultSize }
            , _frameOffset{ 100.0f, 100.0f }
        {
            _min._x = +mint::Math::kFloatMax;
            _min._y = +mint::Math::kFloatMax;

            _max._x = -mint::Math::kFloatMax;
            _max._y = -mint::Math::kFloatMax;
        }

        void Plotter::scatter(const mint::Vector<float>& xData, const mint::Vector<float>& yData)
        {
            _xDataSets.push_back(xData);
            _yDataSets.push_back(yData);

            const uint32 dataCount = xData.size();
            for (uint32 dataIndex = 0; dataIndex < dataCount; ++dataIndex)
            {
                _min._x = mint::min(_min._x, xData[dataIndex]);
                _min._y = mint::min(_min._y, yData[dataIndex]);

                _max._x = mint::max(_max._x, xData[dataIndex]);
                _max._y = mint::max(_max._y, yData[dataIndex]);
            }

            _range = _max - _min;
            
            updateFrameValues();

            _colorArray.push_back(mint::RenderingBase::Color::kTransparent);
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
            const mint::Float2 centerPosition = _position +_size * 0.5f;
            mint::Float4 frameCenterPosition = mint::Float4(centerPosition._x + _frameOffset._x, centerPosition._y + _frameOffset._y, 0.0f, 1.0f);

            drawFrame(frameCenterPosition);
            drawLabels(frameCenterPosition);

            uint32 autoColorIndex = 0;
            const uint32 setCount = _xDataSets.size();
            for (uint32 setIndex = 0; setIndex < setCount; ++setIndex)
            {
                const bool useAutoColor = (_colorArray[setIndex] == mint::RenderingBase::Color::kTransparent);
                const PlotType plotType = _plotTypeArray[setIndex];
                const mint::RenderingBase::Color& color = (useAutoColor == true) ? kAutoColorArray[autoColorIndex] : _colorArray[setIndex];

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
        
        void Plotter::plotScatter(const PlotType plotType, const float x, const float y, const mint::RenderingBase::Color& color)
        {
            const mint::Float2 plotPosition2 = computePlotPosition(x, y);
            
            _shapeFontRendererContext->setColor(color);
            _shapeFontRendererContext->setPosition(mint::Float4(plotPosition2._x, plotPosition2._y, 0.0f, 1.0f));

            switch (plotType)
            {
            case mint::Rendering::Plotter::PlotType::Circle:
            {
                const float kRadius = 4.0f;
                _shapeFontRendererContext->drawCircle(kRadius);
                break;
            }   
            case mint::Rendering::Plotter::PlotType::X:
            {
                const float kLength = 4.0f;
                const float kHalfLength = kLength * 0.5f;
                const float kThickness = 2.0f;
                _shapeFontRendererContext->drawLine(plotPosition2 - mint::Float2(kHalfLength), plotPosition2 + mint::float2(kLength), kThickness);
                _shapeFontRendererContext->drawLine(plotPosition2 - mint::Float2(kHalfLength, -kLength), plotPosition2 + mint::float2(kLength, -kHalfLength), kThickness);
                break;
            }
            case mint::Rendering::Plotter::PlotType::Triangle:
            {
                static const float kSqrt3 = sqrt(3.0f);
                const float kHeight = 8.0f;
                const float kHalfHeight = kHeight * 0.5f;
                const float kHalfWidth = kHeight / kSqrt3;
                _shapeFontRendererContext->drawSolidTriangle(
                    plotPosition2 + mint::Float2(-kHalfWidth, +kHalfHeight), 
                    plotPosition2 + mint::float2(0.0f, -kHalfHeight),
                    plotPosition2 + mint::Float2(+kHalfWidth, +kHalfHeight));
                break;
            }
            default:
                break;
            }
        }

        mint::Float2 Plotter::computeOrigin() const noexcept
        {
            return _frameOffset + mint::Float2(0.0f, _size._y);
        }

        mint::Float2 Plotter::computeInFramePlotPosition(const float x, const float y) const noexcept
        {
            const float xRatio = (x - _frameMin._x) / _frameRange._x;
            const float yRatio = (y - _frameMin._y) / _frameRange._y;
            return mint::Float2(xRatio * _size._x, yRatio * _size._y);
        }

        mint::Float2 Plotter::computePlotPosition(const float x, const float y) const noexcept
        {
            const mint::Float2 inFramePlotPosition = computeInFramePlotPosition(x, y);
            const mint::Float2 origin = computeOrigin();
            return mint::Float2(origin._x + inFramePlotPosition._x, origin._y - inFramePlotPosition._y);
        }

        void Plotter::drawFrame(const mint::Float4& frameCenterPosition) noexcept
        {
            _shapeFontRendererContext->setBorderColor(mint::RenderingBase::Color::kBlack);
            _shapeFontRendererContext->setColor(mint::RenderingBase::Color::kWhite);
            _shapeFontRendererContext->setPosition(frameCenterPosition);
            _shapeFontRendererContext->drawRectangle(_size, 1.0f, 0.0f);
        }

        void Plotter::drawLabels(const mint::Float4& frameCenterPosition) noexcept
        {
            const float paddingY = 10.0f;
            mint::Float4 labelPosition = mint::Float4(frameCenterPosition._x, frameCenterPosition._y + _size._y * 0.5f + paddingY, 0.0f, 1.0f);
            mint::RenderingBase::FontRenderingOption labelRenderingOption;
            labelRenderingOption._directionHorz = mint::RenderingBase::TextRenderDirectionHorz::Centered;
            _shapeFontRendererContext->setTextColor(mint::RenderingBase::Color::kBlack);
            _shapeFontRendererContext->drawDynamicText(_xLabel.c_str(), static_cast<uint32>(_xLabel.length()), labelPosition, labelRenderingOption);

            const float paddingX = -2.0f;
            const float textWidth = _shapeFontRendererContext->calculateTextWidth(_yLabel.c_str(), static_cast<uint32>(_yLabel.length()));
            labelPosition = mint::Float4(frameCenterPosition._x - _size._x * 0.5f - paddingX, frameCenterPosition._y + textWidth * 0.5f, 0.0f, 1.0f);
            labelRenderingOption._transformMatrix = mint::Float4x4::rotationMatrixZ(-mint::Math::kPiOverTwo);
            _shapeFontRendererContext->drawDynamicText(_yLabel.c_str(), static_cast<uint32>(_yLabel.length()), labelPosition, labelRenderingOption);
        }
    }
}
