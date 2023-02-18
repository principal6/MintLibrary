#include <MintRendering/Include/Test.h>
#include <MintRendering/Include/AllHeaders.h>
#include <MintRendering/Include/AllHpps.h>


namespace mint
{
	namespace TestRendering
	{
		bool test(Rendering::GraphicDevice& graphicDevice)
		{
			MINT_ASSURE(Test_SplineGenerator(graphicDevice));
			MINT_ASSURE(Test_Plotter(graphicDevice));
			MINT_ASSURE(Test_MathExpressionRenderer(graphicDevice));
			MINT_ASSURE(Test_GUI(graphicDevice));
			return true;
		}

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
			splineGenerator.setPrecision(16);
			splineGenerator.generateBezierCurve(bezierControlPointSet0, bezierLinePointSet0);
			splineGenerator.generateBezierCurve(bezierControlPointSet1, bezierLinePointSet1);
			splineGenerator.generateBezierCurve(bezierControlPointSet2, bezierLinePointSet2);
			splineGenerator.generateBezierCurve(bezierControlPointSet3, bezierLinePointSet3);
			graphicDevice.getShapeRendererContext().SetColor(Color::kRed);
			graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet0, 1.0f);
			graphicDevice.getShapeRendererContext().SetColor(Color::kGreen);
			graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet1, 1.0f);
			graphicDevice.getShapeRendererContext().SetColor(Color::kBlue);
			graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet2, 1.0f);
			graphicDevice.getShapeRendererContext().SetColor(Color::kCyan);
			graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet3, 1.0f);

			graphicDevice.getShapeRendererContext().SetColor(Color::kBlack);
			for (uint32 sourceControlPointIndex = 0; sourceControlPointIndex < sourceControlPointCount; sourceControlPointIndex++)
			{
				const Float2& sourceControlPoint = sourceControlPointSet[sourceControlPointIndex];
				graphicDevice.getShapeRendererContext().setPosition(Float4(sourceControlPoint._x, sourceControlPoint._y, 0.0, 1.0f));
				graphicDevice.getShapeRendererContext().drawCircle(2.0f);

				if (sourceControlPointIndex > 0)
				{
					const Float2& previousSourceControlPoint = sourceControlPointSet[sourceControlPointIndex - 1];
					graphicDevice.getShapeRendererContext().drawLine(previousSourceControlPoint, sourceControlPoint, 1.0f);
				}
			}

			const uint32 bSplineOrder = 2;
			Vector<float> bSplineKnotVector;
			for (uint32 knotIndex = 0; knotIndex < sourceControlPointCount + bSplineOrder + 1; knotIndex++)
			{
				bSplineKnotVector.PushBack(static_cast<float>(knotIndex));
			}
			Vector<Float2> bSplineLinePointSet;
			splineGenerator.setPrecision(64);
			splineGenerator.generateBSpline(bSplineOrder, sourceControlPointSet, bSplineKnotVector, bSplineLinePointSet);
			graphicDevice.getShapeRendererContext().SetColor(Color::kMagenta);
			graphicDevice.getShapeRendererContext().drawLineStrip(bSplineLinePointSet, 2.0f);
			return true;
		}

		bool Test_Plotter(Rendering::GraphicDevice& graphicDevice)
		{
			using namespace Rendering;

			ShapeRendererContext& shapeFontRendererContext = graphicDevice.getShapeRendererContext();
			Plotter plotter(shapeFontRendererContext);
			plotter.xLabel(L"weight");
			plotter.yLabel(L"length");

			Vector<float> xData{ 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f };
			Vector<float> yData{ 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
			plotter.plotType(Plotter::PlotType::Circle);
			plotter.scatter(xData, yData);

			Vector<float> xData1{ 21.0f, 22.0f, 24.0f, 28.0f, 26.0f, 22.0f };
			Vector<float> yData1{ -2.0f, -3.0f, -8.0f, -1.0f, 50.0f, 30.0f };
			plotter.plotType(Plotter::PlotType::Triangle);
			plotter.scatter(xData1, yData1);

			plotter.render();
			return true;
		}

		bool Test_MathExpressionRenderer(Rendering::GraphicDevice& graphicDevice)
		{
			using namespace Rendering;

			MathExpressionRenderer mathExpressionRenderer(graphicDevice);
			mathExpressionRenderer.drawMathExpression(MathExpression(L"\\bold{aba} is it even possibile? AB=C"), Float2(100, 100));
			mathExpressionRenderer.render();
			return true;
		}

		bool Test_GUI(Rendering::GraphicDevice& graphicDevice)
		{
			using namespace Rendering;
			using namespace GUI;

			GUI::GUIContext& guiContext = graphicDevice.getGUIContext();
			//guiContext._debugSwitch._renderZoneOverlay = true;
			//guiContext._debugSwitch._renderMousePoints = true;
			//guiContext._debugSwitch._renderResizingArea = true;
			ButtonDesc button0Desc;
			button0Desc._text = L"버튼0";
			guiContext.nextControlPosition(Float2(100, 50));
			guiContext.nextControlSize(Float2(100, 30));
			if (guiContext.makeButton(MINT_FILE_LINE, button0Desc))
			{
			}

			WindowDesc window0Desc;
			window0Desc._title = L"윈도우0";
			window0Desc._initialPosition = Float2(100, 100);
			window0Desc._initialSize = Float2(300, 400);
			if (guiContext.beginWindow(MINT_FILE_LINE, window0Desc))
			{
				ButtonDesc button1Desc;
				button1Desc._text = L"버튼1";
				//guiContext.nextControlPosition(Float2(100, 200));
				//guiContext.nextControlSize(Float2(100, 50));
				if (guiContext.makeButton(MINT_FILE_LINE, button1Desc))
				{
				}

				guiContext.nextControlSameLine();

				LabelDesc labelDesc;
				labelDesc._text = L"테스트 레이블";
				//labelDesc.setBackgroundColor(Color::kCyan);
				//labelDesc.setTextColor(Color::kBlack);
				//guiContext.nextControlPosition(Float2(100, 100));
				//guiContext.nextControlSize(Float2(100, 50));
				guiContext.makeLabel(MINT_FILE_LINE, labelDesc);

				guiContext.nextControlSameLine();

				ButtonDesc button2Desc;
				button2Desc._text = L"버튼2";
				if (guiContext.makeButton(MINT_FILE_LINE, button2Desc))
				{
				}

				ButtonDesc button3Desc;
				button3Desc._text = L"버튼3";
				if (guiContext.makeButton(MINT_FILE_LINE, button3Desc))
				{
				}

				guiContext.nextControlSameLine();

				ButtonDesc button4Desc;
				button4Desc._text = L"버튼4";
				if (guiContext.makeButton(MINT_FILE_LINE, button4Desc))
				{
				}

				ButtonDesc button5Desc;
				button5Desc._text = L"버튼5";
				if (guiContext.makeButton(MINT_FILE_LINE, button5Desc))
				{
				}

				guiContext.endWindow();
			}

			ButtonDesc button1Desc;
			button1Desc._text = L"버튼1";
			guiContext.nextControlPosition(Float2(210, 50));
			guiContext.nextControlSize(Float2(100, 30));
			if (guiContext.makeButton(MINT_FILE_LINE, button1Desc))
			{
			}

			WindowDesc window1Desc;
			window1Desc._title = L"윈도우1";
			window1Desc._initialPosition = Float2(450, 100);
			window1Desc._initialSize = Float2(200, 300);
			if (guiContext.beginWindow(MINT_FILE_LINE, window1Desc))
			{

				ButtonDesc button5Desc;
				button5Desc._text = L"버튼1-1";
				if (guiContext.makeButton(MINT_FILE_LINE, button5Desc))
				{
				}

				guiContext.endWindow();
			}

			WindowDesc window2Desc;
			window2Desc._title = L"윈도우2";
			window2Desc._initialPosition = Float2(700, 100);
			window2Desc._initialSize = Float2(200, 200);
			if (guiContext.beginWindow(MINT_FILE_LINE, window2Desc))
			{
				guiContext.endWindow();
			}
			return true;
		}
	}
}
