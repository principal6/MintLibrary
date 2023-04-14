#include <MintRendering/Include/TestRendering.h>
#include <MintRendering/Include/AllHeaders.h>
#include <MintRendering/Include/AllHpps.h>


namespace mint
{
	namespace TestRendering
	{
		bool Test_SplineGenerator(Rendering::GraphicDevice& graphicDevice)
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
			graphicDevice.GetShapeRendererContext().SetColor(Color::kRed);
			graphicDevice.GetShapeRendererContext().DrawLineStrip(bezierLinePointSet0, 1.0f);
			graphicDevice.GetShapeRendererContext().SetColor(Color::kGreen);
			graphicDevice.GetShapeRendererContext().DrawLineStrip(bezierLinePointSet1, 1.0f);
			graphicDevice.GetShapeRendererContext().SetColor(Color::kBlue);
			graphicDevice.GetShapeRendererContext().DrawLineStrip(bezierLinePointSet2, 1.0f);
			graphicDevice.GetShapeRendererContext().SetColor(Color::kCyan);
			graphicDevice.GetShapeRendererContext().DrawLineStrip(bezierLinePointSet3, 1.0f);

			graphicDevice.GetShapeRendererContext().SetColor(Color::kBlack);
			for (uint32 sourceControlPointIndex = 0; sourceControlPointIndex < sourceControlPointCount; sourceControlPointIndex++)
			{
				const Float2& sourceControlPoint = sourceControlPointSet[sourceControlPointIndex];
				graphicDevice.GetShapeRendererContext().SetPosition(Float4(sourceControlPoint._x, sourceControlPoint._y, 0.0, 1.0f));
				graphicDevice.GetShapeRendererContext().DrawCircle(2.0f);

				if (sourceControlPointIndex > 0)
				{
					const Float2& previousSourceControlPoint = sourceControlPointSet[sourceControlPointIndex - 1];
					graphicDevice.GetShapeRendererContext().DrawLine(previousSourceControlPoint, sourceControlPoint, 1.0f);
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
			graphicDevice.GetShapeRendererContext().SetColor(Color::kMagenta);
			graphicDevice.GetShapeRendererContext().DrawLineStrip(bSplineLinePointSet, 2.0f);
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

		bool Test_GUI(Rendering::GUI::GUIContext& guiContext)
		{
			using namespace Rendering;
			using namespace GUI;

			//guiContext._debugSwitch._renderZoneOverlay = true;
			//guiContext._debugSwitch._renderMousePoints = true;
			//guiContext._debugSwitch._renderResizingArea = true;
			ButtonDesc button0Desc;
			button0Desc._text = L"버튼0";
			guiContext.NextControlPosition(Float2(100, 50));
			guiContext.NextControlSize(Float2(100, 30));
			if (guiContext.MakeButton(button0Desc))
			{
			}

			WindowDesc window0Desc;
			window0Desc._title = L"윈도우0";
			window0Desc._initialPosition = Float2(100, 100);
			window0Desc._initialSize = Float2(300, 400);
			if (guiContext.BeginWindow(window0Desc))
			{
				ButtonDesc button1Desc;
				button1Desc._text = L"버튼1";
				//guiContext.NextControlPosition(Float2(100, 200));
				//guiContext.NextControlSize(Float2(100, 50));
				if (guiContext.MakeButton(button1Desc))
				{
				}

				guiContext.NextControlSameLine();

				LabelDesc labelDesc;
				labelDesc._text = L"테스트 레이블";
				//labelDesc.SetBackgroundColor(Color::kCyan);
				//labelDesc.SetTextColor(Color::kBlack);
				//guiContext.NextControlPosition(Float2(100, 100));
				//guiContext.NextControlSize(Float2(100, 50));
				guiContext.MakeLabel(labelDesc);

				guiContext.NextControlSameLine();

				ButtonDesc button2Desc;
				button2Desc._text = L"버튼2";
				if (guiContext.MakeButton(button2Desc))
				{
				}

				ButtonDesc button3Desc;
				button3Desc._text = L"버튼3";
				if (guiContext.MakeButton(button3Desc))
				{
				}

				guiContext.NextControlSameLine();

				ButtonDesc button4Desc;
				button4Desc._text = L"버튼4";
				if (guiContext.MakeButton(button4Desc))
				{
				}

				ButtonDesc button5Desc;
				button5Desc._text = L"버튼5";
				if (guiContext.MakeButton(button5Desc))
				{
				}

				guiContext.EndWindow();
			}

			ButtonDesc button1Desc;
			button1Desc._text = L"버튼1";
			guiContext.NextControlPosition(Float2(210, 50));
			guiContext.NextControlSize(Float2(100, 30));
			if (guiContext.MakeButton(button1Desc))
			{
			}

			WindowDesc window1Desc;
			window1Desc._title = L"윈도우1";
			window1Desc._initialPosition = Float2(450, 100);
			window1Desc._initialSize = Float2(200, 300);
			if (guiContext.BeginWindow(window1Desc))
			{

				ButtonDesc button5Desc;
				button5Desc._text = L"버튼1-1";
				if (guiContext.MakeButton(button5Desc))
				{
				}

				guiContext.EndWindow();
			}

			WindowDesc window2Desc;
			window2Desc._title = L"윈도우2";
			window2Desc._initialPosition = Float2(700, 100);
			window2Desc._initialSize = Float2(200, 200);
			if (guiContext.BeginWindow(window2Desc))
			{
				guiContext.EndWindow();
			}
			return true;
		}
	}
}
