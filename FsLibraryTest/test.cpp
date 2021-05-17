#include <FsLibrary/Include/AllHeaders.h>
#include <FsLibrary/Include/AllHpps.h>


#ifdef FS_DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif


#pragma comment(lib, "FsLibrary.lib")


//#define FS_TEST_FAILURES
//#define FS_TEST_PERFORMANCE


void testIntTypes()
{
    using namespace fs;
    Int2 ni;
    Int2 a{ 1, 2 };
    Int2 b{ 3, 4 };
    Int2 c = a + b;
    Int2 d;
    d = a;
}

void testFloatTypes()
{
    using namespace fs;
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
    
#if defined FS_TEST_FAILURES
    Float2 t;
    t[3] = 1.0f;
#endif
}

void testStaticArray()
{
    using namespace fs;
    constexpr StaticArray<int32, 3> arr{ 4, 5, 999 };

    StaticBitArray<3> ba(true);
    const uint32 bitCount = ba.getBitCount();
    const uint32 byteCount = ba.getByteCount();
    ba.setByte(0, 0xFF);
    ba.setAll(false);
    ba.set(0, true);
    ba.set(3, true);
    ba.setUnsafe(5, true);
    const bool test0 = ba.get(0);
    const bool test1 = ba.get(1);
    const bool test3 = ba.get(3);
    const bool test5 = ba.getUnsafe(5);

    return;
}

void testStackHolder()
{
    using namespace fs;
    {
        StackHolder<16, 6> sh;

        byte* shTestA = sh.registerSpace(2);
        memcpy(shTestA + (16 * 0), "1__abcdefghijk__", 16);
        memcpy(shTestA + (16 * 1), "2__lmnopqrstuv__", 16);

        byte* shTestB = sh.registerSpace(1);
        memcpy(shTestB + (16 * 0), "3__wxyzabcdefg__", 16);

        byte* shTestC = sh.registerSpace(1);
        memcpy(shTestC + (16 * 0), "4__helloMyFrie__", 16);

        sh.deregisterSpace(shTestB);

        byte* shTestD = sh.registerSpace(2);
        memcpy(shTestD + (16 * 0), "5__nd!!IAmLege__", 16);
    }
    {
        StackHolder<8, 12> sh;

        byte* shTestA = sh.registerSpace(7);
        memcpy(shTestA + (8 * 0), "01_abcd_", 8);
        memcpy(shTestA + (8 * 1), "02_efgh_", 8);
        memcpy(shTestA + (8 * 2), "03_ijkl_", 8);
        memcpy(shTestA + (8 * 3), "04_mnop_", 8);
        memcpy(shTestA + (8 * 4), "05_qrst_", 8);
        memcpy(shTestA + (8 * 5), "06_uvwx_", 8);
        memcpy(shTestA + (8 * 6), "07_yzab_", 8);

        byte* shTestB = sh.registerSpace(3);
        memcpy(shTestB + (8 * 0), "08_cdef_", 8);
        memcpy(shTestB + (8 * 1), "09_ghij_", 8);
        memcpy(shTestB + (8 * 2), "10_klmn_", 8);

        sh.deregisterSpace(shTestB);
    }
#ifdef FS_TEST_FAILURES
    {
        StackHolder<0, 0> shA; // THIS MUST FAIL!
        StackHolder<1, 0> shB; // THIS MUST FAIL!
        StackHolder<0, 1> shC; // THIS MUST FAIL!
    }
    {
        StackHolder<32, 16> sh;

        byte* shTestA = sh.registerSpace(16);
        memcpy((char*)(shTestA), "01_abcd_", 8);

        byte* shTestB = sh.registerSpace(8); // THIS MUST FAIL!
        sh.deregisterSpace(shTestB); // THIS MUST FAIL!
    }
#endif
}

const bool testBitVector()
{
    using fs::BitVector;

    BitVector a;
    a.reserveByteCapacity(4);
    a.push_back(true);
    a.push_back(false);
    a.push_back(true);
    a.push_back(false);
    a.push_back(true);
    a.push_back(true);
    a.push_back(true);
    a.push_back(false);
    a.push_back(true);
    a.set(1, true);
    a.set(7, true);
#ifdef FS_TEST_FAILURES
    a.set(10, true);
    a.set(16, true);
#endif
    const bool popped = a.pop_back();
    const bool valueAt2 = a.get(2);
    const bool valueAt3 = a.get(3);
    const bool valueAt4 = a.get(4);
#ifdef FS_TEST_FAILURES
    const bool valueAt5 = a.get(5);
#endif
    
#if defined FS_TEST_PERFORMANCE
    {
#if defined FS_DEBUG
        static constexpr uint32 kCount = 20'000'000;
#else
        static constexpr uint32 kCount = 200'000'000;
#endif

        fs::Vector<uint8> sourceData;
        sourceData.resize(kCount);

        fs::Vector<uint8> byteVector;
        fs::Vector<uint8> byteVectorCopy;
        byteVector.resize(kCount);
        byteVectorCopy.resize(kCount);

        fs::Vector<bool> boolVector;
        fs::Vector<bool> boolVectorCopy;
        boolVector.resize(kCount);
        boolVectorCopy.resize(kCount);

        BitVector bitVector;
        BitVector bitVectorCopy;
        bitVector.resizeBitCount(kCount);
        bitVectorCopy.resizeBitCount(kCount);

        {
            for (uint32 i = 0; i < kCount; ++i)
            {
                sourceData[i] = i % 2;
            }
        }
        
        {
            fs::Profiler::ScopedCpuProfiler profiler{ "1) byte vector" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                byteVector[i] = sourceData[i];
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "1) byte vector copy" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                byteVectorCopy[i] = byteVector[i];
            }
        }
        
        {
            fs::Profiler::ScopedCpuProfiler profiler{ "2) bool vector" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                boolVector[i] = sourceData[i];
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "2) bool vector copy" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                boolVectorCopy[i] = boolVector[i];
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                bitVector.set(i, sourceData[i]);
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector raw copy" };
            for (uint32 i = 0; i < kCount; ++i)
            {
                bitVectorCopy.set(i, bitVector.get(i));
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1" };
            const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
            for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
            {
                const uint32 sourceAt = byteAt * kBitsPerByte;
                for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
                {
                    bitVector.set(byteAt, bitOffset, sourceData[sourceAt + bitOffset]);
                }
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #1 copy" };
            const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
            for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
            {
                const uint32 sourceAt = byteAt * kBitsPerByte;
                for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
                {
                    bitVectorCopy.set(byteAt, bitOffset, bitVector.get(sourceAt + bitOffset));
                }
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2" };
            const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
            for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
            {
                const uint32 sourceAt = byteAt * kBitsPerByte;
                uint8 destByteData = bitVector.getByte(byteAt);
                for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
                {
                    BitVector::setBit(destByteData, bitOffset, sourceData[sourceAt + bitOffset]);
                }
                bitVector.setByte(byteAt, destByteData);
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per bit" };
            const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
            for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
            {
                const uint8 srcByteData = bitVector.getByte(byteAt);
                uint8 destByteData = bitVectorCopy.getByte(byteAt);
                for (uint32 bitOffset = 0; bitOffset < kBitsPerByte; ++bitOffset)
                {
                    BitVector::setBit(destByteData, bitOffset, BitVector::getBit(srcByteData, bitOffset));
                }
                bitVectorCopy.setByte(byteAt, srcByteData);
            }
        }

        {
            fs::Profiler::ScopedCpuProfiler profiler{ "3) bit vector per byte #2 copy per byte" };
            const uint32 kByteCount = BitVector::getByteCountFromBitCount(kCount);
            for (uint32 byteAt = 0; byteAt < kByteCount; ++byteAt)
            {
                const uint8 byteData = bitVector.getByte(byteAt);
                bitVectorCopy.setByte(byteAt, byteData);
            }
        }

        fs::Vector<fs::Profiler::ScopedCpuProfiler::Log> logArray = fs::Profiler::ScopedCpuProfiler::getEntireLogArray();
        const bool isEmpty = logArray.empty();
    }
#endif

    return true;
}

const bool testHashMap()
{
    fs::HashMap<std::string, std::string> hashMap;
    hashMap.insert("1", "a");
    hashMap.insert("5", "b");
    hashMap.insert("11", "c");
    hashMap.insert("21", "d");
    hashMap.insert("33", "e");
    hashMap.insert("41", "f");

    fs::KeyValuePair<std::string, std::string> keyValuePair0 = hashMap.find("1");
    
    hashMap.erase("21");

    fs::KeyValuePair<std::string, std::string> keyValuePair1 = hashMap.find("21");

    return true;
}

const bool testStringTypes()
{
#pragma region ScopeString
    using fs::ScopeStringA;
    {
        ScopeStringA<256> a{ "abcd" };
        ScopeStringA<256> b = a;
        b += b;
        b += b;
        b += b;
        b = "abcdefgh";
        a = b.substr(3);
        const uint32 found0 = b.rfind("def");
        const uint32 found1 = b.rfind("fgh");
        const uint32 found2 = b.rfind("ghi");
        const uint32 found3 = b.rfind("abc");
        const uint32 found4 = b.rfind("abcdc");
        const uint32 found5 = b.rfind("zab");
        if (b == "abcdefgh")
        {
            a = b;
        }
        if (a == b)
        {
            b = "YEAH!";
        }
    }
#pragma endregion

#pragma region UniqueString
    using fs::UniqueStringA;
    using fs::UniqueStringAId;
    {
        UniqueStringA a{ "ab" };
        UniqueStringA b{ "cdef" };
        UniqueStringA c{ "ab" };
        UniqueStringA d{ "" };
        UniqueStringA e{ nullptr };
        UniqueStringA f;
        UniqueStringA g = d;
        UniqueStringA h{ b };
        g = a;
        a = b;
        const bool cmp0 = (a == b);
    }
#pragma endregion

#if defined FS_TEST_PERFORMANCE
    static constexpr uint32 kCount = 20'000;
    {
        fs::Profiler::ScopedCpuProfiler profiler{ "fs::Vector<std::string>" };

        fs::Vector<std::string> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray.set(i, "abcdefg");
        }
    }

    {
        fs::Profiler::ScopedCpuProfiler profiler{ "fs::Vector<std::string>" };

        fs::Vector<std::string> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray[i] = "abcdefg";
        }
    }

    {
        fs::Profiler::ScopedCpuProfiler profiler{ "fs::Vector<std::string>" };

        fs::Vector<std::string> dnsArray;
        dnsArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            dnsArray.set(i, "abcdefg");
        }
    }

    {
        fs::Profiler::ScopedCpuProfiler profiler{ "fs::Vector<std::string>" };

        fs::Vector<std::string> sArray;
        sArray.resize(kCount);
        for (uint32 i = 0; i < kCount; ++i)
        {
            sArray[i] = "abcdefg";
        }
    }

    auto logArray = fs::Profiler::ScopedCpuProfiler::getEntireLogArray();
    const bool isEmpty = logArray.empty();
#endif
#pragma endregion

    return true;
}

const bool testVector()
{
    fs::Vector<uint32> a(5);
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    a.insert(2, 5);
    a.erase(1);

    fs::Vector<uint32> b(20);
    b.push_back(9);

    // Move semantic 점검!
    std::swap(a, b);

    fs::Vector<uint32> c(3);
    c.insert(3, 10);
    c.insert(2, 0);
    c.insert(99, 1);
    c.insert(1, 0);
    c.insert(0, 100);
    c.erase(100);
    c.erase(2);
    c.shrink_to_fit();

    return true;
}

const bool testStringUtil()
{
    const std::string testA{ "ab c   def g" };
    fs::Vector<std::string> testATokenized;
    fs::StringUtil::tokenize(testA, ' ', testATokenized);
    
    std::string testB{
        R"(
            #include <ShaderStructDefinitions>
            #include <VsConstantBuffers>

            VS_OUTPUT_COLOR main(VS_INPUT_COLOR input)
            {
                VS_OUTPUT_COLOR result = (VS_OUTPUT_COLOR)0;
                result._position    = mul(float4(input._position.xyz, 1.0), _cb2DProjectionMatrix);
                result._color       = input._color;
                result._texCoord    = input._texCoord;
                result._flag        = input._flag;
                return result;
            }
        )"
    };
    const fs::Vector<char> delimiterArray{ ' ', '\t', '\n' };
    fs::Vector<std::string> testBTokenized;
    fs::StringUtil::tokenize(testB, delimiterArray, testBTokenized);

    return true;
}

const bool testTree()
{
    fs::Tree<std::string> stringTree;
    fs::TreeNodeAccessor rootNode = stringTree.createRootNode("ROOT");
    
    fs::TreeNodeAccessor a = rootNode.insertChildNode("A");
    const std::string& aData = a.getNodeData();
    
    fs::TreeNodeAccessor b = a.insertChildNode("b");
    fs::TreeNodeAccessor c = a.insertChildNode("c");

    fs::TreeNodeAccessor d = rootNode.insertChildNode("D");

    //stringTree.swapNodeData(a, b);
    b.moveToParent(rootNode);
    fs::TreeNodeAccessor bParent = b.getParentNode();
    const uint32 aChildCount = a.getChildNodeCount();
    a.clearChildNodes();
#if defined FS_TEST_FAILURES
    fs::TreeNodeAccessor aInvalidChild = a.getChildNode(10);
#endif

    fs::TreeNodeAccessor found = stringTree.findNode(rootNode, "A");

    //stringTree.eraseChildNode(rootNode, a);
    //stringTree.clearChildren(rootNode);
    stringTree.destroyRootNode();

#if defined FS_TEST_FAILURES
    stringTree.moveToParent(rootNode, d);
#endif

    return true;
}

const bool testFiles()
{
    static constexpr const char* const kFileName = "FsLibraryTest/test.bin";
    static constexpr const char* const kRawString = "abc";
    fs::BinaryFileWriter bfw;
    bfw.write(3.14f);
    bfw.write(true);
    bfw.write(static_cast<uint16>(0xABCD));
    bfw.write("hello");
    bfw.write(kRawString);
    bfw.save(kFileName);

    fs::BinaryFileReader bfr;
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

    fs::TextFileReader tfr;
    tfr.open("FsLibraryTest/test.cpp");

    return true;
}

const bool testLanguage()
{
    using namespace fs;
    
    TextFileReader textFileReader;
    textFileReader.open("Assets/CppHlsl/CppHlslStreamData.h");
    
    Language::CppHlslLexer cppHlslLexer{ textFileReader.get() };
    cppHlslLexer.execute();
    
    Language::CppHlslParser cppHlslParser{ cppHlslLexer };
    cppHlslParser.execute();

    struct TestStruct
    {
        fs::float1 _a           = 1.0f;                             // v[0]
        fs::float1 _b           = 2.0f;
        fs::float2 _padding0    = fs::float2(4.0f, 8.0f);
        fs::float3 _c           = fs::float3(16.0f, 32.0f, 64.0f);  // v[1]
        fs::float1 _padding1    = 128.0f;
    };
    fs::RenderingBase::VS_INPUT_SHAPE vsInput;
    uint64 a = sizeof(fs::RenderingBase::VS_INPUT_SHAPE);
    uint64 b = sizeof(fs::RenderingBase::VS_OUTPUT_SHAPE);
    uint64 c = sizeof(fs::RenderingBase::CB_View);
    TestStruct ts;
    uint64 tss = sizeof(TestStruct);

    std::string syntaxTreeString = cppHlslParser.getSyntaxTreeString();
    const fs::Language::CppHlslTypeInfo& typeInfo0 = cppHlslParser.getTypeInfo("VS_INPUT_SHAPE");
    const fs::Language::CppHlslTypeInfo& typeInfo1 = cppHlslParser.getTypeInfo(1);
    const fs::Language::CppHlslTypeInfo& typeInfo2 = cppHlslParser.getTypeInfo(2);
    return true;
}

const bool testAlgorithm()
{
    fs::Vector<uint32> a;
    a.push_back(4);
    a.push_back(3);
    a.push_back(0);
    a.push_back(2);
    a.push_back(1);

    fs::quickSort(a, fs::ComparatorAscending<uint32>());
    fs::quickSort(a, fs::ComparatorDescending<uint32>());
    
    return true;
}

//#pragma optimize("", off)
const bool testLinearAlgebra()
{
    fs::Math::VectorR<3> vec0(1.0, 1.0, 0.0);
    vec0 = 5 * vec0;
    fs::Math::VectorR<3> vec1(0.0, 3.0, 0.0);
    fs::Math::VectorR<3> vec2 = vec0.cross(vec1).setNormalized();
    const bool trueValue = vec2.isUnitVector();
    const bool falseValue = fs::Math::equals(1.00002f, 1.0f);
    const double distance = vec1.normalize().distance(vec2);
    const double theta = vec1.angle(vec2);
    const bool orthogonality = vec1.isOrthogonalTo(vec2);

    fs::Math::VectorR<1> vec3(3.0);
    fs::Math::Matrix<1, 3> mat0;
    mat0.setRow(0, fs::Math::VectorR<3>(4.0, 5.0, 6.0));
    constexpr bool isMat0Square = mat0.isSquareMatrix();
    
    fs::Math::Matrix<3, 3> mat1;
    mat1.setRow(0, fs::Math::VectorR<3>(3.0, 0.0, 0.0));
    mat1.setRow(1, fs::Math::VectorR<3>(0.0, 3.0, 0.0));
    mat1.setRow(2, fs::Math::VectorR<3>(0.0, 0.0, 3.0));
    const bool isMat1Scalar = mat1.isScalarMatrix();
    mat1.setIdentity();
    const bool isMat1Identity = mat1.isIdentityMatrix();
    mat1.setZero();
    const bool isMat1Zero = mat1.isZeroMatrix();

    fs::Math::VectorR<3> a = fs::Math::VectorR<3>(1.0, 2.0, 3.0);
    mat1.setRow(0, fs::Math::VectorR<3>(1.0, 2.0, 3.0));
    mat1.setRow(1, fs::Math::VectorR<3>(4.0, 5.0, 6.0));
    mat1.setRow(2, fs::Math::VectorR<3>(7.0, 8.0, 9.0));
    fs::Math::VectorR<3> e1 = fs::Math::VectorR<3>::standardUnitVector(1);
    fs::Math::VectorR<3> row1 = e1 * mat1;
    fs::Math::VectorR<3> col1 = mat1 * e1;
    vec0 = vec3 * mat0;

    mat1.setRow(1, fs::Math::VectorR<3>(2.0, 5.0, 6.0));
    mat1.setRow(2, fs::Math::VectorR<3>(3.0, 6.0, 9.0));
    const bool isMat1Symmetric = mat1.isSymmetricMatrix();

    fs::Math::Matrix<2, 3> mat2;
    mat2.setRow(0, fs::Math::VectorR<3>(0.0, 1.0, 2.0));
    mat2.setRow(1, fs::Math::VectorR<3>(3.0, 4.0, 5.0));
    fs::Math::Matrix<3, 2> mat2Transpose = mat2.transpose();
    return true;
}

const bool testWindow()
{
    using namespace fs;
    using namespace fs::Window;

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

    RenderingBase::GraphicDevice graphicDevice;
    graphicDevice.initialize(&window);

    fs::Gui::GuiContext& guiContext = graphicDevice.getGuiContext();

    Rendering::MeshRenderer meshRenderer{ &graphicDevice };
    meshRenderer.initialize();
    
    Rendering::ObjectPool objectPool;
    Rendering::Object* const testObject = objectPool.createObject();
    Rendering::CameraObject* const testCameraObject = objectPool.createCameraObject();
    fs::Float2 windowSize = graphicDevice.getWindowSizeFloat2();
    testCameraObject->setPerspectiveZRange(0.01f, 1000.0f);
    testCameraObject->setPerspectiveScreenRatio(windowSize._x / windowSize._y);
    {
        testObject->attachComponent(objectPool.createMeshComponent());

        fs::Rendering::TransformComponent* transformComponent = static_cast<fs::Rendering::TransformComponent*>(testObject->getComponent(fs::Rendering::ObjectComponentType::TransformComponent));
        transformComponent->_srt._translation._z = 4.0f;
        //transformComponent->_srt._rotation.setAxisAngle(fs::Float3(1.0f, 1.0f, 0.0f), fs::Math::kPiOverEight);
    }
    testCameraObject->rotatePitch(0.125f);

    uint64 previousFrameTimeMs = 0;
    while (window.isRunning() == true)
    {
        objectPool.computeDeltaTime();

        // Events
        {
            guiContext.handleEvents(&window);

            if (window.hasEvent() == true)
            {
                EventData event = window.popEvent();
                if (event._type == EventType::KeyDown)
                {
                    if (event._value.isKeyCode(EventData::KeyCode::Enter) == true)
                    {
                        graphicDevice.getShaderPool().recompileAllShaders();
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num1) == true)
                    {
                        graphicDevice.useSolidCullBackRasterizer();
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num2) == true)
                    {
                        graphicDevice.useWireFrameCullBackRasterizer();
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num3) == true)
                    {
                        graphicDevice.useWireFrameNoCullingRasterizer();
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num4) == true)
                    {
                        fs::Rendering::MeshComponent* const meshComponent = static_cast<fs::Rendering::MeshComponent*>(testObject->getComponent(fs::Rendering::ObjectComponentType::MeshComponent));
                        meshComponent->shouldDrawNormals(!meshComponent->shouldDrawNormals());
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Num5) == true)
                    {
                        fs::Rendering::MeshComponent* const meshComponent = static_cast<fs::Rendering::MeshComponent*>(testObject->getComponent(fs::Rendering::ObjectComponentType::MeshComponent));
                        meshComponent->shouldDrawEdges(!meshComponent->shouldDrawEdges());
                    }
                    else if (event._value.isKeyCode(EventData::KeyCode::Shift) == true)
                    {
                        testCameraObject->setIsBoostMode(true);
                    }
                }
                else if (event._type == EventType::KeyUp)
                {
                    if (event._value.isKeyCode(EventData::KeyCode::Shift) == true)
                    {
                        testCameraObject->setIsBoostMode(false);
                    }
                }
                else if (event._type == EventType::MouseWheel)
                {
                    const float mouseWheel = event._value.getMouseWheel();
                    if (0.0f < mouseWheel)
                    {
                        testCameraObject->increaseMoveSpeed();
                    }
                    else
                    {
                        testCameraObject->decreaseMoveSpeed();
                    }
                }
                else if (event._type == EventType::MouseMoveDelta)
                {
                    if (window.isMouseDown(fs::Window::EventData::MouseButton::Right) == true)
                    {
                        const fs::Float2& mouseDeltaPosition = event._value.getAndClearMouseDeltaPosition();
                        testCameraObject->rotatePitch(mouseDeltaPosition._y);
                        testCameraObject->rotateYaw(mouseDeltaPosition._x);
                    }
                }
                else if (event._type == EventType::WindowResized)
                {
                    graphicDevice.updateScreenSize();
                    guiContext.updateScreenSize(graphicDevice.getWindowSizeFloat2());
                    objectPool.updateScreenSize(graphicDevice.getWindowSizeFloat2());
                }
            }
        }

        // Dynamic Keyboard Inputs
        if (guiContext.isFocusedControlTextBox() == false)
        {
            if (window.isKeyDown(EventData::KeyCode::Q) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Upward);
            }

            if (window.isKeyDown(EventData::KeyCode::E) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Downward);
            }

            if (window.isKeyDown(EventData::KeyCode::W) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Forward);
            }

            if (window.isKeyDown(EventData::KeyCode::S) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Backward);
            }

            if (window.isKeyDown(EventData::KeyCode::A) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Leftward);
            }

            if (window.isKeyDown(EventData::KeyCode::D) == true)
            {
                testCameraObject->move(fs::Rendering::CameraObject::MoveDirection::Rightward);
            }
        }

        // Rendering
        {
            graphicDevice.beginRendering();

#if 0
            fs::RenderingBase::ShapeFontRendererContext& shapeFontRendererContext = graphicDevice.getShapeFontRendererContext();
            fs::Rendering::Plotter plotter(shapeFontRendererContext);
            plotter.xLabel(L"weight");
            plotter.yLabel(L"length");
            
            fs::Vector<float> xData{  1.0f,  2.0f,  4.0f,  8.0f, 16.0f, 32.0f };
            fs::Vector<float> yData{ 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f };
            plotter.plotType(fs::Rendering::Plotter::PlotType::Circle);
            plotter.scatter(xData, yData);

            fs::Vector<float> xData1{ 21.0f, 22.0f, 24.0f, 28.0f, 26.0f, 22.0f };
            fs::Vector<float> yData1{ -2.0f, -3.0f, -8.0f, -1.0f, 50.0f, 30.0f };
            plotter.plotType(fs::Rendering::Plotter::PlotType::Triangle);
            plotter.scatter(xData1, yData1);

            plotter.render();
#endif
#if 1
            {
                static fs::Gui::VisibleState testWindowVisibleState = fs::Gui::VisibleState::Invisible;
                guiContext.testWindow(testWindowVisibleState);
                if (guiContext.beginMenuBar(L"MainMenuBar") == true)
                {
                    if (guiContext.beginMenuBarItem(L"파일") == true)
                    {
                        if (guiContext.beginMenuItem(L"종료") == true)
                        {
                            if (guiContext.isControlPressed() == true)
                            {
                                window.destroy();
                            }
                            guiContext.endMenuItem();
                        }
                        guiContext.endMenuBarItem();
                    }

                    if (guiContext.beginMenuBarItem(L"윈도우") == true)
                    {
                        if (guiContext.beginMenuItem(L"TestWindow") == true)
                        {
                            if (guiContext.isControlPressed() == true)
                            {
                                testWindowVisibleState = fs::Gui::VisibleState::VisibleOpen;
                            }
                            
                            guiContext.endMenuItem();
                        }

                        guiContext.endMenuBarItem();
                    }

                    guiContext.endMenuBar();
                }

                fs::Gui::WindowParam inspectorWindowParam;
                inspectorWindowParam._size = fs::Float2(320.0f, 400.0f);
                inspectorWindowParam._position = fs::Float2(20.0f, 50.0f);
                inspectorWindowParam._initialDockingMethod = fs::Gui::DockingMethod::RightSide;
                inspectorWindowParam._initialDockingSize._x = 320.0f;
                static fs::Gui::VisibleState inspectorVisibleState;
                if (guiContext.beginWindow(L"Inspector", inspectorWindowParam, inspectorVisibleState) == true)
                {
                    wchar_t tempBuffer[256];
                    fs::Gui::LabelParam fpsLabelParam;
                    fpsLabelParam._fontColor = fs::RenderingBase::Color(200, 220, 255, 255);
                    fpsLabelParam._alignmentHorz = fs::Gui::TextAlignmentHorz::Left;
                    
                    fs::formatString(tempBuffer, L" FPS: %d", fs::Profiler::FpsCounter::getFps());
                    guiContext.pushLabel(L"FPS_Label", tempBuffer, fpsLabelParam);

                    fs::formatString(tempBuffer, L" CPU: %d ms", fs::Profiler::FpsCounter::getFrameTimeMs());
                    guiContext.pushLabel(L"CPU_Label", tempBuffer, fpsLabelParam);
                    
                    fs::formatString(tempBuffer, L" Camera Position: %.3f, %.3f, %.3f", 
                        testCameraObject->getObjectTransformSrt()._translation._x,
                        testCameraObject->getObjectTransformSrt()._translation._y, 
                        testCameraObject->getObjectTransformSrt()._translation._z);
                    guiContext.pushLabel(L"Camera Position", tempBuffer, fpsLabelParam);
                    guiContext.endWindow();
                }
            }

            graphicDevice.setViewMatrix(testCameraObject->getViewMatrix());
            graphicDevice.setProjectionMatrix(testCameraObject->getProjectionMatrix());
            graphicDevice.updateViewProjectionMatrix();

            meshRenderer.render(objectPool);
#endif

            graphicDevice.endRendering();
        }
        
        fs::Profiler::FpsCounter::count();
    }
    return true;
}

const bool testAll()
{
    //fs::Logger::setOutputFileName("LOG.txt");
/*
*/
    testIntTypes();

    testFloatTypes();

    testStaticArray();

    testStackHolder();

    testBitVector();

    testHashMap();

    /*
    */
    testStringTypes();

    testVector();

    testStringUtil();

    testTree();

    FS_LOG("김장원", "LOG %d", 1234);
    //FS_ASSERT("김장원", false, "ASSERTION");
    //FS_LOG_ERROR("김장원", "ERROR");

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
#ifdef FS_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    fs::FsLibraryVersion::printVersion();

#if defined FS_DEBUG
    #if defined FS_TEST_PERFORMANCE
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
