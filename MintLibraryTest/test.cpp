﻿#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>


#include <MintLibraryTest/TestContainers.h>
#include <MintLibraryTest/TestReflection.h>


#ifdef MINT_DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif


#pragma comment(lib, "MintLibrary.lib")

#pragma optimize("", off)


//#define MINT_TEST_FAILURES
//#define MINT_TEST_PERFORMANCE


void testIntTypes()
{
    using namespace mint;
    Int2 ni;
    Int2 a{ 1, 2 };
    Int2 b{ 3, 4 };
    Int2 c = a + b;
    Int2 d;
    d = a;
}

void testFloatTypes()
{
    using namespace mint;
    float a = 15.000001f;
    float b = 15.000002f;
    float c = 15000.0001f;
    float d = 15000.0005f;
    float e = 15000.0015f;

    struct
    {
        Float3 _a;
        Float3 _b;
    } st;
    auto sizeFloat2 = sizeof(Float2);
    auto sizeFloat3 = sizeof(Float3);
    auto sizeFloat4 = sizeof(Float4);
    auto sizeSt = sizeof(st);
    Float3 p{ 1, 0, 0 };
    Float3 q{ 0, 1, 0 };
    Float3 r = Float3::cross(p, q);
    
#if defined MINT_TEST_FAILURES
    Float2 t;
    t[3] = 1.0f;
#endif
    {
        /*
        static constexpr uint64 kTestCount = 500'000'000;
        {
            Profiler::ScopedCPUProfiler profiler("Profile - Float4");
            Float4 a(1.0f, 0.0f, 1.0f, 0.0f);
            Float4 b(0.0f, 1.0f, 0.0f, 1.0f);
            Float4 c;
            for (uint64 i = 0; i < kTestCount; ++i)
            {
                //a += b;
                //a -= b;
                //a *= 0.5f;
                //a *= 2.0f;
                c = a + b;
                //c = Float4::cross(a, b);
            }
        }
        {
            Profiler::ScopedCPUProfiler profiler("Profile - AffineVecF");
            AffineVecF a(1.0f, 0.0f, 1.0f, 0.0f);
            AffineVecF b(0.0f, 1.0f, 0.0f, 1.0f);
            AffineVecF c;
            for (uint64 i = 0; i < kTestCount; ++i)
            {
                //a += b;
                //a -= b;
                //a *= 0.5f;
                //a *= 2.0f;
                c = a + b;
                //c = a.cross(b);
            }
        }
        auto logArray = Profiler::ScopedCPUProfiler::getEntireLogArray();
        */

        AffineVecF a;
        AffineVecF b;
        AffineVecF c = AffineVecF(0.0f, 2.0f, 3.0f, 4.0f);
        a == b;
        a == c;

        AffineVecD da;
        AffineVecD db;
        AffineVecD dc = AffineVecD(0, 2, 3, 4);
        da == db;
        da += dc;
        auto sizeA = sizeof(a);
        auto sizeB = sizeof(da);
        printf("");
    }
}

const bool testFiles()
{
    using namespace mint;

    static constexpr const char* const kFileName = "MintLibraryTest/test.bin";
    static constexpr const char* const kRawString = "abc";
    BinaryFileWriter bfw;
    bfw.write(3.14f);
    bfw.write(true);
    bfw.write(static_cast<uint16>(0xABCD));
    bfw.write("hello");
    bfw.write(kRawString);
    bfw.save(kFileName);

    BinaryFileReader bfr;
    bfr.open(kFileName);
    if (bfr.isOpen() == true)
    {
        auto a = bfr.read<float>();
        auto b = bfr.read<bool>();
        auto c = bfr.read<uint16>();
        auto d = bfr.read<char>(6);
        auto e = bfr.read<char>(4);
        printf("File[%s] %s %s\n", kFileName, d, e);
    }

    TextFileReader tfr;
    tfr.open("MintLibraryTest/test.cpp");

    return true;
}

const bool testLanguage()
{
    using namespace mint;
    using namespace Language;
    
    TextFileReader textFileReader;
    if (textFileReader.open(Path::makeAssetPath("CppHlsl/CppHlslStreamData.h")) == false)
    {
        return false;
    }

    CppHlsl::Lexer cppHlslLexer{ textFileReader.get() };
    cppHlslLexer.execute();
    
    CppHlsl::Parser cppHlslParser{ cppHlslLexer };
    cppHlslParser.execute();

    struct TestStruct
    {
        float1 _a           = 1.0f;                         // v[0]
        float1 _b           = 2.0f;
        float2 _padding0    = float2(4.0f, 8.0f);
        float3 _c           = float3(16.0f, 32.0f, 64.0f);  // v[1]
        float1 _padding1    = 128.0f;
    };
    Rendering::VS_INPUT_SHAPE vsInput;
    uint64 a = sizeof(Rendering::VS_INPUT_SHAPE);
    uint64 b = sizeof(Rendering::VS_OUTPUT_SHAPE);
    uint64 c = sizeof(Rendering::CB_View);
    TestStruct ts;
    uint64 tss = sizeof(TestStruct);

    //std::string syntaxTreeString = cppHlslParser.getSyntaxTreeString();
    //const TypeMetaData& typeMetaData0 = cppHlslParser.getTypeMetaData("VS_INPUT_SHAPE");
    //const TypeMetaData& typeMetaData1 = cppHlslParser.getTypeMetaData(1);
    //const TypeMetaData& typeMetaData2 = cppHlslParser.getTypeMetaData(2);
    return true;
}

const bool testAlgorithm()
{
    using namespace mint;

    Vector<uint32> a;
    a.push_back(4);
    a.push_back(3);
    a.push_back(0);
    a.push_back(2);
    a.push_back(1);

    quickSort(a, ComparatorAscending<uint32>());
    quickSort(a, ComparatorDescending<uint32>());
    
    return true;
}

const bool testLinearAlgebra()
{
    using namespace mint;
    
    VectorD<3> vec0(1.0, 1.0, 0.0);
    vec0 = 5.0 * vec0;
    VectorD<3> vec1(0.0, 3.0, 0.0);
    VectorD<3> vec2 = cross(vec0, vec1).setNormalized();
    const bool trueValue = vec2.isUnitVector();
    const bool falseValue = Math::equals(1.00002f, 1.0f);
    const double distance = vec1.normalize().distance(vec2);
    const double theta = vec1.angle(vec2);
    const bool orthogonality = vec1.isOrthogonalTo(vec2);

    VectorD<1> vec3(3.0);
    MatrixD<1, 3> mat0;
    mat0.setRow(0, VectorD<3>(4.0, 5.0, 6.0));
    constexpr bool isMat0Square = mat0.isSquareMatrix();
    
    MatrixD<3, 3> mat1;
    mat1.setRow(0, VectorD<3>(3.0, 0.0, 0.0));
    mat1.setRow(1, VectorD<3>(0.0, 3.0, 0.0));
    mat1.setRow(2, VectorD<3>(0.0, 0.0, 3.0));
    const bool isMat1Scalar = mat1.isScalarMatrix();
    mat1.setIdentity();
    const bool isMat1Identity = mat1.isIdentityMatrix();
    mat1.setZero();
    const bool isMat1Zero = mat1.isZeroMatrix();

    VectorD<3> a = VectorD<3>(1.0, 2.0, 3.0);
    mat1.setRow(0, VectorD<3>(1.0, 2.0, 3.0));
    mat1.setRow(1, VectorD<3>(4.0, 5.0, 6.0));
    mat1.setRow(2, VectorD<3>(7.0, 8.0, 9.0));
    VectorD<3> e1 = VectorD<3>::standardUnitVector(1);
    VectorD<3> row1 = e1 * mat1;
    VectorD<3> col1 = mat1 * e1;
    vec0 = vec3 * mat0;

    mat1.setRow(1, VectorD<3>(2.0, 5.0, 6.0));
    mat1.setRow(2, VectorD<3>(3.0, 6.0, 9.0));
    const bool isMat1Symmetric = mat1.isSymmetricMatrix();

    mat1.setRow(1, VectorD<3>(-2.0, 5.0, 6.0));
    mat1.setRow(2, VectorD<3>(-3.0, -6.0, 9.0));
    const bool isMat1SkewSymmetric = mat1.isSkewSymmetricMatrix();

    MatrixD<2, 3> mat2;
    mat2.setRow(0, VectorD<3>(0.0, 1.0, 2.0));
    mat2.setRow(1, VectorD<3>(3.0, 4.0, 5.0));
    MatrixD<3, 2> mat2Transpose = mat2.transpose();

    MatrixD<2, 2> mat3;
    const bool isMat3Idempotent = mat3.isIdempotentMatrix();


    Float4x4 testFloat4x4;
    testFloat4x4.set(1, 1, 1, 0, 0, 3, 1, 2, 2, 3, 1, 0, 1, 0, 2, 1);
    Float4x4 testFloat4x4Inverse{ testFloat4x4.inverse() };

    Matrix<4, 4, float> testMatrix4x4;
    testMatrix4x4.setRow(0, {  1, 1, 1, 0 });
    testMatrix4x4.setRow(1, {  0, 3, 1, 2 });
    testMatrix4x4.setRow(2, {  2, 3, 1, 0 });
    testMatrix4x4.setRow(3, {  1, 0, 2, 1 });
    const bool testEquals = testMatrix4x4 == testMatrix4x4;
    testMatrix4x4 *= testMatrix4x4;

    const Matrix<4, 4, float> testIdentity(MatrixUtils::identity<4, float>());

    // Affine
    {
        float v[4];
        AffineVecF vec0 = AffineVecF(1, 0, 0, 0);
        AffineVecF vec1 = AffineVecF(0, 1, 0, 0);
        AffineVecF vec2 = vec0.cross(vec1);
        vec0.setComponent(3, 1);
        vec0.get(v);

        AffineMat<float> mat0 = translationMatrix(AffineVecF(4, 5, 6, 1));
        AffineMat<float> mat1 = scalarMatrix(AffineVecF(2, 3, 4, 0));
        mat1 *= mat0;
        AffineVecF vec3 = mat1 * vec0;
        AffineMat<float> mat1Inv = mat1.inverse();
        mat1 *= mat1Inv;

        AffineMat<float> mat2 = rotationMatrixAxisAngle(AffineVecF(1, 0, 0, 0), 1.0f);
        printf("");
    }
    
    return true;
}

const bool testWindow()
{
    using namespace mint;
    using namespace Window;

    CreationData windowCreationData;
    windowCreationData._style = Style::Default;
    windowCreationData._position.set(200, 100);
    windowCreationData._size.set(1024, 768);
    windowCreationData._title = L"HI";
    windowCreationData._bgColor.set(0.875f, 0.875f, 0.875f);

    WindowsWindow window;
    if (window.create(windowCreationData) == false)
    {
        CreationError error = window.getCreationError();
        return false;
    }

    Rendering::GraphicDevice graphicDevice(window);
    graphicDevice.initialize();

    //Rendering::MathExpressionRenderer mathExpressionRenderer(graphicDevice);
    GUI::GUIContext& guiContext = graphicDevice.getGUIContext();
    Platform::InputContext& inputContext = Platform::InputContext::getInstance();

    Rendering::ObjectPool objectPool;
    Rendering::Object* const testObject = objectPool.createObject();
    Rendering::CameraObject* const testCameraObject = objectPool.createCameraObject();
    Float2 windowSize = graphicDevice.getWindowSizeFloat2();
    testCameraObject->setPerspectiveZRange(0.01f, 1000.0f);
    testCameraObject->setPerspectiveScreenRatio(windowSize._x / windowSize._y);
    {
        testObject->attachComponent(objectPool.createMeshComponent());
        
        Transform& transform = testObject->getObjectTransform();
        transform._translation._z = -4.0f;
    }
    testCameraObject->rotatePitch(0.125f);
    
    Rendering::MeshRenderer meshRenderer{ graphicDevice };
    Rendering::InstantRenderer instantRenderer{ graphicDevice };
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

        const bool isInputBoxFocused = guiContext.isFocusedControlInputBox();
        if (isInputBoxFocused == false)
        {
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
                    Rendering::MeshComponent* const meshComponent = static_cast<Rendering::MeshComponent*>(testObject->getComponent(Rendering::ObjectComponentType::MeshComponent));
                    meshComponent->shouldDrawNormals(!meshComponent->shouldDrawNormals());
                }
                else if (inputContext.isKeyDown(Platform::KeyCode::Num5) == true)
                {
                    Rendering::MeshComponent* const meshComponent = static_cast<Rendering::MeshComponent*>(testObject->getComponent(Rendering::ObjectComponentType::MeshComponent));
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
        }
        
        if (window.isResized())
        {
            graphicDevice.updateScreenSize();
            guiContext.updateScreenSize(graphicDevice.getWindowSizeFloat2());
            objectPool.updateScreenSize(graphicDevice.getWindowSizeFloat2());
        }

        const bool isCameraMoveLocked = isInputBoxFocused;
        testCameraObject->steer(inputContext, isCameraMoveLocked);

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
            Rendering::SplineGenerator splineGenerator;
            Vector<Float2> bezierLinePointSet0;
            Vector<Float2> bezierLinePointSet1;
            Vector<Float2> bezierLinePointSet2;
            Vector<Float2> bezierLinePointSet3;
            splineGenerator.setPrecision(16);
            splineGenerator.generateBezierCurve(bezierControlPointSet0, bezierLinePointSet0);
            splineGenerator.generateBezierCurve(bezierControlPointSet1, bezierLinePointSet1);
            splineGenerator.generateBezierCurve(bezierControlPointSet2, bezierLinePointSet2);
            splineGenerator.generateBezierCurve(bezierControlPointSet3, bezierLinePointSet3);
            graphicDevice.getShapeRendererContext().setColor(Rendering::Color::kRed);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet0, 1.0f);
            graphicDevice.getShapeRendererContext().setColor(Rendering::Color::kGreen);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet1, 1.0f);
            graphicDevice.getShapeRendererContext().setColor(Rendering::Color::kBlue);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet2, 1.0f);
            graphicDevice.getShapeRendererContext().setColor(Rendering::Color::kCyan);
            graphicDevice.getShapeRendererContext().drawLineStrip(bezierLinePointSet3, 1.0f);

            graphicDevice.getShapeRendererContext().setColor(Rendering::Color::kBlack);
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
            graphicDevice.getShapeRendererContext().setColor(Rendering::Color::kMagenta);
            graphicDevice.getShapeRendererContext().drawLineStrip(bSplineLinePointSet, 2.0f);
#endif
#if 0 // Plotter
            Rendering::ShapeRendererContext& shapeFontRendererContext = graphicDevice.getShapeRendererContext();
            Rendering::Plotter plotter(shapeFontRendererContext);
            plotter.xLabel(L"weight");
            plotter.yLabel(L"length");
            
            Vector<float> xData{  1.0f,  2.0f,  4.0f,  8.0f, 16.0f, 32.0f };
            Vector<float> yData{ 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
            plotter.plotType(Rendering::Plotter::PlotType::Circle);
            plotter.scatter(xData, yData);

            Vector<float> xData1{ 21.0f, 22.0f, 24.0f, 28.0f, 26.0f, 22.0f };
            Vector<float> yData1{ -2.0f, -3.0f, -8.0f, -1.0f, 50.0f, 30.0f };
            plotter.plotType(Rendering::Plotter::PlotType::Triangle);
            plotter.scatter(xData1, yData1);

            plotter.render();
#endif

// GUI
#if 1
            {
                GUI::ControlMetaStateSet& controlMetaStateSet = guiContext.getControlMetaStateSet();

                static GUI::VisibleState testWindowVisibleState = GUI::VisibleState::Invisible;
                static GUI::VisibleState debugControlDataViewerVisibleState = GUI::VisibleState::Invisible;
                guiContext.makeTestWindow(testWindowVisibleState);
                guiContext.makeDebugControlDataViewer(debugControlDataViewerVisibleState);
                if (guiContext.beginMenuBar(MINT_FILE_LINE(), L"") == true)
                {
                    if (guiContext.beginMenuBarItem(MINT_FILE_LINE(), L"파일") == true)
                    {
                        if (guiContext.beginMenuItem(MINT_FILE_LINE(), L"종료") == true)
                        {
                            if (guiContext.isThisControlPressed() == true)
                            {
                                window.destroy();
                            }
                            guiContext.endMenuItem();
                        }
                        guiContext.endMenuBarItem();
                    }

                    if (guiContext.beginMenuBarItem(MINT_FILE_LINE(), L"윈도우") == true)
                    {
                        if (guiContext.beginMenuItem(MINT_FILE_LINE(), L"TestWindow") == true)
                        {
                            if (guiContext.isThisControlPressed() == true)
                            {
                                testWindowVisibleState = GUI::VisibleState::VisibleOpen;
                            }
                            
                            guiContext.endMenuItem();
                        }

                        if (guiContext.beginMenuItem(MINT_FILE_LINE(), L"ControlData Viewer") == true)
                        {
                            if (guiContext.isThisControlPressed() == true)
                            {
                                debugControlDataViewerVisibleState = GUI::VisibleState::VisibleOpen;
                            }

                            guiContext.endMenuItem();
                        }

                        guiContext.endMenuBarItem();
                    }

                    guiContext.endMenuBar();
                }

                GUI::WindowParam inspectorWindowParam;
                controlMetaStateSet.nextSize(Float2(320.0f, 400.0f));
                inspectorWindowParam._initialDockZone = GUI::DockZone::RightSide;
                inspectorWindowParam._initialDockingSize._x = 320.0f;
                static GUI::VisibleState inspectorVisibleState;
                if (guiContext.beginWindow(MINT_FILE_LINE(), L"Inspector", inspectorWindowParam, inspectorVisibleState) == true)
                {
                    wchar_t tempBuffer[256];
                    GUI::LabelParam labelParam;
                    labelParam._common._fontColor = Rendering::Color(200, 220, 255, 255);
                    labelParam._alignmentHorz = GUI::TextAlignmentHorz::Left;
                    
                    formatString(tempBuffer, L" FPS: %d", Profiler::FPSCounter::getFps());
                    guiContext.makeLabel(MINT_FILE_LINE(), tempBuffer, labelParam);

                    formatString(tempBuffer, L" CPU: %d ms", Profiler::FPSCounter::getFrameTimeMs());
                    guiContext.makeLabel(MINT_FILE_LINE(), tempBuffer, labelParam);
                    
                    Float3& cameraPosition = testCameraObject->getObjectTransform()._translation;
                    guiContext.makeLabel(MINT_FILE_LINE(), L" Camera Position:", labelParam);
                    
                    {
                        labelParam._common._backgroundColor.r(1.0f);
                        labelParam._common._backgroundColor.a(0.75f);
                        labelParam._common._fontColor = Rendering::Color::kWhite;
                        GUI::CommonControlParam valueSliderParam;
                        const float maxWidth = guiContext.getCurrentAvailableDisplaySizeX();
                        const float labelWidth = 16.0f;
                        const Float2 valueSliderSize = Float2((maxWidth - guiContext.getCurrentSameLineIntervalX() * 2.0f) / 3.0f, 24.0f);
                        controlMetaStateSet.pushSize(valueSliderSize);
                        {
                            if (guiContext.beginLabeledValueSlider(MINT_FILE_LINE(), L"X", labelParam, valueSliderParam, labelWidth, 0.0f, 3, cameraPosition._x) == true)
                            {
                                guiContext.endLabeledValueSlider();
                            }

                            controlMetaStateSet.nextSameLine();

                            labelParam._common._backgroundColor.r(0.0f);
                            labelParam._common._backgroundColor.g(0.875f);
                            if (guiContext.beginLabeledValueSlider(MINT_FILE_LINE(), L"Y", labelParam, valueSliderParam, labelWidth, 0.0f, 3, cameraPosition._y) == true)
                            {
                                guiContext.endLabeledValueSlider();
                            }

                            controlMetaStateSet.nextSameLine();

                            labelParam._common._backgroundColor.g(0.0f);
                            labelParam._common._backgroundColor.b(1.0f);
                            if (guiContext.beginLabeledValueSlider(MINT_FILE_LINE(), L"Z", labelParam, valueSliderParam, labelWidth, 0.0f, 3, cameraPosition._z) == true)
                            {
                                guiContext.endLabeledValueSlider();
                            }
                        }
                        controlMetaStateSet.popSize();
                    }
                    
                    guiContext.endWindow();
                }
            }
#endif

#if 1
            testSkeleton.renderSkeleton(instantRenderer, testSkeletonWorldMatrix);

            graphicDevice.setViewProjectionMatrix(testCameraObject->getViewMatrix(), testCameraObject->getProjectionMatrix());

            meshRenderer.render(objectPool);
            
            // # ShapeRendererContext 테스트
            //Rendering::ShapeRendererContext& shapeFontRendererContext = graphicDevice.getShapeRendererContext();
            //shapeFontRendererContext.testDraw(Float2(30, 60));

            // # InstantRenderer 테스트
            //instantRenderer.testDraw(Float3::kZero);
            instantRenderer.render();

            //mathExpressionRenderer.drawMathExpression(Rendering::MathExpression(L"\\bold{aba} is it even possibile? AB=C"), Float2(100, 100));
            //mathExpressionRenderer.render();
#endif

            graphicDevice.endRendering();
        }
        
        Profiler::FPSCounter::count();
    }
    return true;
}

const bool testAll()
{
    using namespace mint;

    //Logger::setOutputFileName("LOG.txt");

    testIntTypes();

    testFloatTypes();

    TestContainers::testAll();

    testReflection();

    MINT_LOG("LOG %d", 1234);
    //MINT_ASSERT(false, "ASSERTION");
    //MINT_LOG_ERROR("ERROR");

    /*
    */
    testFiles();

    testLanguage();

    testAlgorithm();

    testLinearAlgebra();

    return true;
}

int main()
{
#ifdef MINT_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    mint::Library::initialize();

#if defined MINT_DEBUG
    #if defined MINT_TEST_PERFORMANCE
        //testMemoryAllocator();
        testBitVector();
        //testStringTypes();
    #else
        testAll();
        
    #endif
#else
    HWND handleToConsoleWindow = ::GetConsoleWindow();
    ::FreeConsole();
    ::SendMessageW(handleToConsoleWindow, WM_CLOSE, 0, 0);
#endif
    
    testWindow();

    return 0;
}
