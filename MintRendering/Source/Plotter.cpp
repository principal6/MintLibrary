#include <MintRendering/Include/Plotter.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintContainer/Include/StringUtil.hpp>

#include <MintRenderingBase/Include/ShapeRendererContext.h>


namespace mint
{
	namespace Rendering
	{
		Plotter::Plotter(ShapeRendererContext& shapeFontRendererContext)
			: _shapeFontRendererContext{ &shapeFontRendererContext }
			, _nextPlotType{ PlotType::Circle }
			, _size{ kDefaultSize }
			, _frameOffset{ 100.0f, 100.0f }
		{
			_min._x = +Math::kFloatMax;
			_min._y = +Math::kFloatMax;

			_max._x = -Math::kFloatMax;
			_max._y = -Math::kFloatMax;
		}

		void Plotter::Clear()
		{
			_xDataSets.Clear();
			_yDataSets.Clear();
			_colorArray.Clear();
			_plotTypeArray.Clear();
		}

		void Plotter::Scatter(const Vector<float>& xData, const Vector<float>& yData)
		{
			_xDataSets.PushBack(xData);
			_yDataSets.PushBack(yData);

			const uint32 dataCount = xData.Size();
			for (uint32 dataIndex = 0; dataIndex < dataCount; ++dataIndex)
			{
				_min._x = Min(_min._x, xData[dataIndex]);
				_min._y = Min(_min._y, yData[dataIndex]);

				_max._x = Max(_max._x, xData[dataIndex]);
				_max._y = Max(_max._y, yData[dataIndex]);
			}

			_range = _max - _min;

			UpdateFrameValues();

			_colorArray.PushBack(Color::kTransparent);
			_plotTypeArray.PushBack(_nextPlotType);
		}

		void Plotter::UpdateFrameValues()
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

		void Plotter::Render() noexcept
		{
			const Float2 centerPosition = _position + _size * 0.5f;
			Float4 frameCenterPosition = Float4(centerPosition._x + _frameOffset._x, centerPosition._y + _frameOffset._y, 0.0f, 1.0f);

			DrawFrame(frameCenterPosition);
			DrawLabels(frameCenterPosition);

			uint32 autoColorIndex = 0;
			const uint32 setCount = _xDataSets.Size();
			for (uint32 setIndex = 0; setIndex < setCount; ++setIndex)
			{
				const bool useAutoColor = (_colorArray[setIndex] == Color::kTransparent);
				const PlotType plotType = _plotTypeArray[setIndex];
				const Color& color = (useAutoColor == true) ? kAutoColorArray[autoColorIndex] : _colorArray[setIndex];

				const uint32 dataCount = _xDataSets[setIndex].Size();
				for (uint32 dataIndex = 0; dataIndex < dataCount; ++dataIndex)
				{
					const float xDatum = _xDataSets[setIndex][dataIndex];
					const float yDatum = _yDataSets[setIndex][dataIndex];

					PlotScatter(plotType, xDatum, yDatum, color);
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

			_shapeFontRendererContext->Render();
			_shapeFontRendererContext->Flush();

			Clear();
		}

		void Plotter::PlotScatter(const PlotType plotType, const float x, const float y, const Color& color)
		{
			const Float2 plotPosition2 = ComputePlotPosition(x, y);

			_shapeFontRendererContext->SetColor(color);
			_shapeFontRendererContext->SetPosition(Float4(plotPosition2._x, plotPosition2._y, 0.0f, 1.0f));

			switch (plotType)
			{
			case Plotter::PlotType::Circle:
			{
				const float kRadius = 4.0f;
				_shapeFontRendererContext->DrawCircle(kRadius);
				break;
			}
			case Plotter::PlotType::X:
			{
				const float kLength = 4.0f;
				const float kHalfLength = kLength * 0.5f;
				const float kThickness = 2.0f;
				_shapeFontRendererContext->DrawLine(plotPosition2 - Float2(kHalfLength), plotPosition2 + float2(kLength), kThickness);
				_shapeFontRendererContext->DrawLine(plotPosition2 - Float2(kHalfLength, -kLength), plotPosition2 + float2(kLength, -kHalfLength), kThickness);
				break;
			}
			case Plotter::PlotType::Triangle:
			{
				static const float kSqrt3 = sqrt(3.0f);
				const float kHeight = 8.0f;
				const float kHalfHeight = kHeight * 0.5f;
				const float kHalfWidth = kHeight / kSqrt3;
				_shapeFontRendererContext->DrawSolidTriangle(
					plotPosition2 + Float2(-kHalfWidth, +kHalfHeight),
					plotPosition2 + float2(0.0f, -kHalfHeight),
					plotPosition2 + Float2(+kHalfWidth, +kHalfHeight));
				break;
			}
			default:
				break;
			}
		}

		Float2 Plotter::ComputeOrigin() const noexcept
		{
			return _frameOffset + Float2(0.0f, _size._y);
		}

		Float2 Plotter::ComputeInFramePlotPosition(const float x, const float y) const noexcept
		{
			const float xRatio = (x - _frameMin._x) / _frameRange._x;
			const float yRatio = (y - _frameMin._y) / _frameRange._y;
			return Float2(xRatio * _size._x, yRatio * _size._y);
		}

		Float2 Plotter::ComputePlotPosition(const float x, const float y) const noexcept
		{
			const Float2 inFramePlotPosition = ComputeInFramePlotPosition(x, y);
			const Float2 origin = ComputeOrigin();
			return Float2(origin._x + inFramePlotPosition._x, origin._y - inFramePlotPosition._y);
		}

		void Plotter::DrawFrame(const Float4& frameCenterPosition) noexcept
		{
			_shapeFontRendererContext->SetShapeBorderColor(Color::kBlack);
			_shapeFontRendererContext->SetColor(Color::kWhite);
			_shapeFontRendererContext->SetPosition(frameCenterPosition);
			_shapeFontRendererContext->DrawRectangle(_size, 1.0f, 0.0f);
		}

		void Plotter::DrawLabels(const Float4& frameCenterPosition) noexcept
		{
			const float paddingY = 10.0f;
			Float4 labelPosition = Float4(frameCenterPosition._x, frameCenterPosition._y + _size._y * 0.5f + paddingY, 0.0f, 1.0f);
			FontRenderingOption labelRenderingOption;
			labelRenderingOption._directionHorz = TextRenderDirectionHorz::Centered;
			_shapeFontRendererContext->SetTextColor(Color::kBlack);
			_shapeFontRendererContext->DrawDynamicText(_xLabel.c_str(), static_cast<uint32>(_xLabel.length()), labelPosition, labelRenderingOption);

			const float paddingX = -2.0f;
			const float textWidth = _shapeFontRendererContext->GetFontData().ComputeTextWidth(_yLabel.c_str(), static_cast<uint32>(_yLabel.length()));
			labelPosition = Float4(frameCenterPosition._x - _size._x * 0.5f - paddingX, frameCenterPosition._y + textWidth * 0.5f, 0.0f, 1.0f);
			labelRenderingOption._transformMatrix = Float4x4::RotationMatrixZ(-Math::kPiOverTwo);
			_shapeFontRendererContext->DrawDynamicText(_yLabel.c_str(), static_cast<uint32>(_yLabel.length()), labelPosition, labelRenderingOption);
		}
	}
}
