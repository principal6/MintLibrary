#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>


#include <MintMath/Include/Test.h>
#include <MintContainer/Include/Test.h>
#include <MintPlatform/Include/Test.h>
#include <MintLanguage/Include/Test.h>
#include <MintReflection/Include/Test.h>
#include <MintLibrary/Include/Test.h>


#ifdef MINT_DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif


#pragma comment(lib, "MintLibrary.lib")

#pragma optimize("", off)


//#define MINT_TEST_FAILURES
//#define MINT_TEST_PERFORMANCE


bool runTestWindow()
{
    using namespace mint;
    using namespace Window;
    using namespace Rendering;

    WindowCreationDesc windowCreationDesc;
    windowCreationDesc._style = Style::Default;
    windowCreationDesc._position.set(200, 100);
    windowCreationDesc._size.set(1024, 768);
    windowCreationDesc._title = L"HI";
    windowCreationDesc._backgroundColor.set(0.875f, 0.875f, 0.875f);

    WindowsWindow window;
    if (window.create(windowCreationDesc) == false)
    {
        WindowCreationError windowCreationError = window.getWindowCreationError();
        return false;
    }

    GraphicDevice graphicDevice{ window };
    graphicDevice.initialize();

    //MathExpressionRenderer mathExpressionRenderer(graphicDevice);
    GUI::GUIContext& guiContext = graphicDevice.getGUIContext();
    Platform::InputContext& inputContext = Platform::InputContext::getInstance();

    ObjectPool objectPool;
    Object* const testObject = objectPool.createObject();
    CameraObject* const testCameraObject = objectPool.createCameraObject();
    Float2 windowSize = graphicDevice.getWindowSizeFloat2();
    testCameraObject->setPerspectiveZRange(0.01f, 1000.0f);
    testCameraObject->setPerspectiveScreenRatio(windowSize._x / windowSize._y);
    {
        testObject->attachComponent(objectPool.createMeshComponent());
        
        Transform& transform = testObject->getObjectTransform();
        transform._translation._z = -4.0f;
    }
    testCameraObject->rotatePitch(0.125f);
    
    MeshRenderer meshRenderer{ graphicDevice };
    InstantRenderer instantRenderer{ graphicDevice };
    Game::SkeletonGenerator testSkeletonGenerator;
    Float4x4 testSkeletonWorldMatrix;
    testSkeletonWorldMatrix.setTranslation(1.0f, 0.0f, -4.0f);
    Float4x4 bindPoseLocalMatrix;
    testSkeletonGenerator.createJoint(-1, "Root", bindPoseLocalMatrix);
    bindPoseLocalMatrix.setTranslation(1.0f, 0.0f, 0.0f);
    testSkeletonGenerator.createJoint(0, "Elbow", bindPoseLocalMatrix);
    bindPoseLocalMatrix.setTranslation(1.0f, 0.0f, 0.0f);
    testSkeletonGenerator.createJoint(1, "Tip", bindPoseLocalMatrix);
    testSkeletonGenerator.buildBindPoseModelSpace();
    Game::Skeleton testSkeleton(testSkeletonGenerator);
    
    uint64 previousFrameTimeMs = 0;
    while (window.isRunning() == true)
    {
        objectPool.computeDeltaTime();

        // Events
        inputContext.processEvents();
        guiContext.processEvent();

        if (inputContext.isKeyPressed())
        {
            if (inputContext.isKeyDown(Platform::KeyCode::Enter) == true)
            {
                graphicDevice.getShaderPool().recompileAllShaders();
            }
            else if (inputContext.isKeyDown(Platform::KeyCode::Num1) == true)
            {
                graphicDevice.useSolidCullBackRasterizer();
            }
            else if (inputContext.isKeyDown(Platform::KeyCode::Num2) == true)
            {
                graphicDevice.useWireFrameCullBackRasterizer();
            }
            else if (inputContext.isKeyDown(Platform::KeyCode::Num3) == true)
            {
                graphicDevice.useWireFrameNoCullingRasterizer();
            }
            else if (inputContext.isKeyDown(Platform::KeyCode::Num4) == true)
            {
                MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->getComponent(ObjectComponentType::MeshComponent));
                meshComponent->shouldDrawNormals(!meshComponent->shouldDrawNormals());
            }
            else if (inputContext.isKeyDown(Platform::KeyCode::Num5) == true)
            {
                MeshComponent* const meshComponent = static_cast<MeshComponent*>(testObject->getComponent(ObjectComponentType::MeshComponent));
                meshComponent->shouldDrawEdges(!meshComponent->shouldDrawEdges());
            }
            else if (inputContext.isKeyDown(Platform::KeyCode::Shift) == true)
            {
                testCameraObject->setBoostMode(true);
            }
        }
        else if (inputContext.isKeyReleased())
        {
            if (inputContext.isKeyUp(Platform::KeyCode::Shift) == true)
            {
                testCameraObject->setBoostMode(false);
            }
        }
        else if (inputContext.isMouseWheelScrolled())
        {
            const float mouseWheelScroll = inputContext.getMouseWheelScroll();
            if (mouseWheelScroll > 0.0f)
            {
                testCameraObject->increaseMoveSpeed();
            }
            else
            {
                testCameraObject->decreaseMoveSpeed();
            }
        }
        
        if (window.isResized())
        {
            graphicDevice.updateScreenSize();
            guiContext.updateScreenSize(graphicDevice.getWindowSizeFloat2());
            objectPool.updateScreenSize(graphicDevice.getWindowSizeFloat2());
        }

        testCameraObject->steer(inputContext, false);

        // Rendering
        {
            graphicDevice.beginRendering();

#if 0 // SplineGenerator
            Vector<Float2> sourceControlPointSet;
            sourceControlPointSet.push_back(Float2(30, 100));
            sourceControlPointSet.push_back(Float2(50, 200));
            sourceControlPointSet.push_back(Float2(100, 200));
            sourceControlPointSet.push_back(Float2(110, 100));
            sourceControlPointSet.push_back(Float2(160, 200));
            sourceControlPointSet.push_back(Float2(200, 100));
            sourceControlPointSet.push_back(Float2(250, 200));
            const uint32 sourceControlPointCount = sourceControlPointSet.size();
            Vector<Float2> bezierControlPointSet0;
            bezierControlPointSet0.push_back(sourceControlPointSet[0]);
            bezierControlPointSet0.push_back(sourceControlPointSet[1]);
            bezierControlPointSet0.push_back(sourceControlPointSet[2]);
            bezierControlPointSet0.push_back(sourceControlPointSet[3]);
            Vector<Float2> bezierControlPointSet1;
            bezierControlPointSet1.push_back(sourceControlPointSet[1]);
            bezierControlPointSet1.push_back(sourceControlPointSet[2]);
            bezierControlPointSet1.push_back(sourceControlPointSet[3]);
            bezierControlPointSet1.push_back(sourceControlPointSet[4]);
            Vector<Float2> bezierControlPointSet2;
            bezierControlPointSet2.push_back(sourceControlPointSet[2]);
            bezierControlPointSet2.push_back(sourceControlPointSet[3]);
            bezierControlPointSet2.push_back(sourceControlPointSet[4]);
            bezierControlPointSet2.push_back(sourceControlPointSet[5]);
            Vector<Float2> bezierControlPointSet3;
            bezierControlPointSet3.push_back(sourceControlPointSet[3]);
            bezierControlPointSet3.push_back(sourceControlPointSet[4]);
            bezierControlPointSet3.push_back(sourceControlPointSet[5]);
            bezierControlPointSet3.push_back(sourceControlPointSet[6]);
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
            graphicDevice.getShapeRendererContext().setColor(Color::kRed);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet0, 1.0f);
            graphicDevice.getShapeRendererContext().setColor(Color::kGreen);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet1, 1.0f);
            graphicDevice.getShapeRendererContext().setColor(Color::kBlue);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet2, 1.0f);
            graphicDevice.getShapeRendererContext().setColor(Color::kCyan);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet3, 1.0f);

            graphicDevice.getShapeRendererContext().setColor(Color::kBlack);
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
                bSplineKnotVector.push_back(static_cast<float>(knotIndex));
            }
            Vector<Float2> bSplineLinePointSet;
            splineGenerator.setPrecision(64);
            splineGenerator.generateBSpline(bSplineOrder, sourceControlPointSet, bSplineKnotVector, bSplineLinePointSet);
            graphicDevice.getShapeRendererContext().setColor(Color::kMagenta);
            graphicDevice.getShapeRendererContext().drawLineStrip(bSplineLinePointSet, 2.0f);
#endif
#if 0 // Plotter
            ShapeRendererContext& shapeFontRendererContext = graphicDevice.getShapeRendererContext();
            Plotter plotter(shapeFontRendererContext);
            plotter.xLabel(L"weight");
            plotter.yLabel(L"length");
            
            Vector<float> xData{  1.0f,  2.0f,  4.0f,  8.0f, 16.0f, 32.0f };
            Vector<float> yData{ 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
            plotter.plotType(Plotter::PlotType::Circle);
            plotter.scatter(xData, yData);

            Vector<float> xData1{ 21.0f, 22.0f, 24.0f, 28.0f, 26.0f, 22.0f };
            Vector<float> yData1{ -2.0f, -3.0f, -8.0f, -1.0f, 50.0f, 30.0f };
            plotter.plotType(Plotter::PlotType::Triangle);
            plotter.scatter(xData1, yData1);

            plotter.render();
#endif

// GUI
#if 0
            using namespace GUI;
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
#endif

#if 1
            testSkeleton.renderSkeleton(instantRenderer, testSkeletonWorldMatrix);

            graphicDevice.setViewProjectionMatrix(testCameraObject->getViewMatrix(), testCameraObject->getProjectionMatrix());

            meshRenderer.render(objectPool);
            
            // # ShapeRendererContext 테스트
            //ShapeRendererContext& shapeFontRendererContext = graphicDevice.getShapeRendererContext();
            //shapeFontRendererContext.testDraw(Float2(30, 60));

            // # InstantRenderer 테스트
            //instantRenderer.testDraw(Float3::kZero);
            instantRenderer.render();

            //mathExpressionRenderer.drawMathExpression(MathExpression(L"\\bold{aba} is it even possibile? AB=C"), Float2(100, 100));
            //mathExpressionRenderer.render();
#endif

            graphicDevice.endRendering();
        }
        
        Profiler::FPSCounter::count();
    }
    return true;
}

int main()
{
    using namespace mint;
#ifdef MINT_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Library::initialize();

#if defined MINT_DEBUG
    #if defined MINT_TEST_PERFORMANCE
        //testMemoryAllocator();
        testBitVector();
        //testStringTypes();
    #else
        //Logger::setOutputFileName("LOG.txt");
        TestMath::test();
        TestContainers::test();
        TestReflection::test();
        TestPlatform::test();
        TestLanguage::test();
        TestLibrary::test();
    #endif
#else
    HWND handleToConsoleWindow = ::GetConsoleWindow();
    ::FreeConsole();
    ::SendMessageW(handleToConsoleWindow, WM_CLOSE, 0, 0);
#endif
    
    runTestWindow();
    return 0;
}
