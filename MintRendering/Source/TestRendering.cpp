#include <MintRendering/Include/TestRendering.h>
#include <MintRenderingBase/Include/AllHeaders.h>
#include <MintRenderingBase/Include/AllHpps.h>
#include <MintRendering/Include/AllHeaders.h>


namespace mint
{
	namespace TestRendering
	{
		bool Test_SplineGenerator(Rendering::GraphicsDevice& graphicsDevice)
		{
			using namespace Rendering;

			Vector<Float2> sourceControlPointSet;
			sourceControlPointSet.PushBack(Float2(30, 100));
			sourceControlPointSet.PushBack(Float2(50, 200));
			sourceControlPointSet.PushBack(Float2(100, 200));
			sourceControlPointSet.PushBack(Float2(110, 100));
			sourceControlPointSet.PushBack(Float2(160, 200));
			sourceControlPointSet.PushBack(Float2(200, 100));
			sourceControlPointSet.PushBack(Float2(250, 200));
			const uint32 sourceControlPointCount = sourceControlPointSet.Size();
			Vector<Float2> bezierControlPointSet0;
			bezierControlPointSet0.PushBack(sourceControlPointSet[0]);
			bezierControlPointSet0.PushBack(sourceControlPointSet[1]);
			bezierControlPointSet0.PushBack(sourceControlPointSet[2]);
			bezierControlPointSet0.PushBack(sourceControlPointSet[3]);
			Vector<Float2> bezierControlPointSet1;
			bezierControlPointSet1.PushBack(sourceControlPointSet[1]);
			bezierControlPointSet1.PushBack(sourceControlPointSet[2]);
			bezierControlPointSet1.PushBack(sourceControlPointSet[3]);
			bezierControlPointSet1.PushBack(sourceControlPointSet[4]);
			Vector<Float2> bezierControlPointSet2;
			bezierControlPointSet2.PushBack(sourceControlPointSet[2]);
			bezierControlPointSet2.PushBack(sourceControlPointSet[3]);
			bezierControlPointSet2.PushBack(sourceControlPointSet[4]);
			bezierControlPointSet2.PushBack(sourceControlPointSet[5]);
			Vector<Float2> bezierControlPointSet3;
			bezierControlPointSet3.PushBack(sourceControlPointSet[3]);
			bezierControlPointSet3.PushBack(sourceControlPointSet[4]);
			bezierControlPointSet3.PushBack(sourceControlPointSet[5]);
			bezierControlPointSet3.PushBack(sourceControlPointSet[6]);
			SplineGenerator splineGenerator;
			Vector<Float2> bezierLinePointSet0;
			Vector<Float2> bezierLinePointSet1;
			Vector<Float2> bezierLinePointSet2;
			Vector<Float2> bezierLinePointSet3;
			splineGenerator.SetPrecision(16);
			splineGenerator.GenerateBezierCurve(bezierControlPointSet0, bezierLinePointSet0);
			splineGenerator.GenerateBezierCurve(bezierControlPointSet1, bezierLinePointSet1);
			splineGenerator.GenerateBezierCurve(bezierControlPointSet2, bezierLinePointSet2);
			splineGenerator.GenerateBezierCurve(bezierControlPointSet3, bezierLinePointSet3);

			ShapeRendererContext& shapeRendererContext = graphicsDevice.GetShapeRendererContext();
			shapeRendererContext.SetColor(Color::kRed);
			shapeRendererContext.DrawLineStrip(bezierLinePointSet0, 1.0f);
			shapeRendererContext.SetColor(Color::kGreen);
			shapeRendererContext.DrawLineStrip(bezierLinePointSet1, 1.0f);
			shapeRendererContext.SetColor(Color::kBlue);
			shapeRendererContext.DrawLineStrip(bezierLinePointSet2, 1.0f);
			shapeRendererContext.SetColor(Color::kCyan);
			shapeRendererContext.DrawLineStrip(bezierLinePointSet3, 1.0f);

			shapeRendererContext.SetColor(Color::kBlack);
			for (uint32 sourceControlPointIndex = 0; sourceControlPointIndex < sourceControlPointCount; sourceControlPointIndex++)
			{
				const Float2& sourceControlPoint = sourceControlPointSet[sourceControlPointIndex];
				shapeRendererContext.SetPosition(Float4(sourceControlPoint._x, sourceControlPoint._y, 0.0, 1.0f));
				shapeRendererContext.DrawCircle(2.0f);

				if (sourceControlPointIndex > 0)
				{
					const Float2& previousSourceControlPoint = sourceControlPointSet[sourceControlPointIndex - 1];
					shapeRendererContext.DrawLine(previousSourceControlPoint, sourceControlPoint, 1.0f);
				}
			}

			const uint32 bSplineOrder = 2;
			Vector<float> bSplineKnotVector;
			for (uint32 knotIndex = 0; knotIndex < sourceControlPointCount + bSplineOrder + 1; knotIndex++)
			{
				bSplineKnotVector.PushBack(static_cast<float>(knotIndex));
			}
			Vector<Float2> bSplineLinePointSet;
			splineGenerator.SetPrecision(64);
			splineGenerator.GenerateBSpline(bSplineOrder, sourceControlPointSet, bSplineKnotVector, bSplineLinePointSet);
			shapeRendererContext.SetColor(Color::kMagenta);
			shapeRendererContext.DrawLineStrip(bSplineLinePointSet, 2.0f);
			return true;
		}

		bool Test_Plotter(Rendering::Plotter& plotter)
		{
			using namespace Rendering;

			plotter.Clear();

			plotter.SetXLabel(L"weight");
			plotter.SetYLabel(L"length");

			Vector<float> xData{ 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f };
			Vector<float> yData{ 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
			plotter.SetPlotType(Plotter::PlotType::Circle);
			plotter.Scatter(xData, yData);

			Vector<float> xData1{ 21.0f, 22.0f, 24.0f, 28.0f, 26.0f, 22.0f };
			Vector<float> yData1{ -2.0f, -3.0f, -8.0f, -1.0f, 50.0f, 30.0f };
			plotter.SetPlotType(Plotter::PlotType::Triangle);
			plotter.Scatter(xData1, yData1);

			plotter.Render();
			return true;
		}

		bool Test_MathExpressionRenderer(Rendering::MathExpressionRenderer& mathExpressionRenderer)
		{
			using namespace Rendering;

			mathExpressionRenderer.drawMathExpression(MathExpression(L"\\bold{aba} is it even possibile? AB=C"), Float2(100, 100));
			mathExpressionRenderer.Render();
			return true;
		}
	}
}
