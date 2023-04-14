#pragma once


#ifndef _MINT_RENDERING_PLOTTER_H_
#define _MINT_RENDERING_PLOTTER_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintMath/Include/Float2.h>


namespace mint
{
	namespace Rendering
	{
		class ShapeRendererContext;


		class Plotter
		{
			static constexpr Float2 kDefaultSize = Float2(400, 300);
			static constexpr Color kAutoColorArray[]{
				Color(0.0f, 0.5f, 1.0f),
				Color(1.0f, 0.125f, 0.0625f),
				Color(1.0f, 0.625f, 0.0f),
				Color(0.125f, 0.75f, 0.15f),
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
			Plotter(ShapeRendererContext& shapeFontRendererContext);
			~Plotter() = default;

		public:
			void Clear();
			void Scatter(const Vector<float>& xData, const Vector<float>& yData);

		private:
			void UpdateFrameValues();

		public:
			MINT_INLINE void SetPlotType(const PlotType nextPlotType) noexcept { _nextPlotType = nextPlotType; }
			MINT_INLINE void SetXLabel(const std::wstring& label) noexcept { _xLabel = label; }
			MINT_INLINE void SetYLabel(const std::wstring& label) noexcept { _yLabel = label; }
			void Render() noexcept;

		private:
			void PlotScatter(const PlotType plotType, const float x, const float y, const Color& color);
			Float2 ComputeOrigin() const noexcept;
			Float2 ComputeInFramePlotPosition(const float x, const float y) const noexcept;
			Float2 ComputePlotPosition(const float x, const float y) const noexcept;
			void DrawFrame(const Float4& frameCenterPosition) noexcept;
			void DrawLabels(const Float4& frameCenterPosition) noexcept;

		private:
			ShapeRendererContext* const _shapeFontRendererContext;

		private:
			Vector<Vector<float>> _xDataSets;
			Vector<Vector<float>> _yDataSets;
			Vector<Color> _colorArray;

		private:
			Vector<PlotType> _plotTypeArray;
			PlotType _nextPlotType;

		private:
			Float2 _min;
			Float2 _max;
			Float2 _range;
			Float2 _framePadding;
			Float2 _frameMin;
			Float2 _frameMax;
			Float2 _frameRange;

		private:
			std::wstring _xLabel;
			std::wstring _yLabel;
			Float2 _position;
			Float2 _size;
			Float2 _frameOffset;
		};
	}
}


#endif // !_MINT_RENDERING_PLOTTER_H_
